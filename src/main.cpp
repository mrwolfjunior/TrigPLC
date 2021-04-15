/*
 * Created by Emanuele Bono
 * 
 * TODO:
 * - Insert #define DEBUG as class (?)
 *
 * POSSIBLE IN/OUT:
 * 
 * byte inputs[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
 *                  CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9,
 *                  CONTROLLINO_A10, CONTROLLINO_A11, CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14,
 *                  CONTROLLINO_A15, CONTROLLINO_I16, CONTROLLINO_I17, CONTROLLINO_I18, CONTROLLINO_IN0,
 *                  CONTROLLINO_IN1};
 * 
 * byte outputs[] = {CONTROLLINO_D0, CONTROLLINO_D1, CONTROLLINO_D2, CONTROLLINO_D3, CONTROLLINO_D4,
 *                   CONTROLLINO_D5, CONTROLLINO_D6, CONTROLLINO_D7, CONTROLLINO_D8, CONTROLLINO_D9,
 *                   CONTROLLINO_D10, CONTROLLINO_D11, CONTROLLINO_D12, CONTROLLINO_D13, CONTROLLINO_D14,
 *                   CONTROLLINO_D15, CONTROLLINO_D16, CONTROLLINO_D17, CONTROLLINO_D18, CONTROLLINO_D19};
 * 
 * byte relays[] = {CONTROLLINO_R0, CONTROLLINO_R1, CONTROLLINO_R2, CONTROLLINO_R3, CONTROLLINO_R4,
 *                  CONTROLLINO_R5, CONTROLLINO_R6, CONTROLLINO_R7, CONTROLLINO_R8, CONTROLLINO_R9,
 *                  CONTROLLINO_R10, CONTROLLINO_R11, CONTROLLINO_R12, CONTROLLINO_R13, CONTROLLINO_R14,
 *                  CONTROLLINO_R15};
*/
#include <Arduino_FreeRTOS.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <config.h>
#include <semphr.h> // add the FreeRTOS functions for Semaphores (or Flags).


// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// FreeRTOS variable and function
void TaskTrigger(void *pvParameters);
void TaskIOT(void *pvParameters);
TaskHandle_t TaskIOTHandle; // handler for TaskIOT

EthernetClient ethClient;
long MQTT_TIMEOUT = 10000/portTICK_PERIOD_MS;

PubSubClient mqttClient(ethClient);
long lastMQTTConnection;
StaticJsonDocument<256> staticJsonDocument;
char jsonBuffer[256];
char stateOnJson[128];
char stateOffJson[128];

void handleMQTTConnection();
void publishToMQTT(const char *p_topic, char *p_payload);
void subscribeToMQTT(const char *p_topic);
void handleMQTTMessage(char *p_topic, byte *p_payload, unsigned int p_length);

void setup()
{

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if (xSerialSemaphore == NULL) // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex(); // Create a mutex semaphore we will use to manage the Serial Port
    if ((xSerialSemaphore) != NULL)
      xSemaphoreGive((xSerialSemaphore)); // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  xTaskCreate(
      TaskTrigger, "Trigger" // A name just for humans
      ,
      128 // This stack size can be checked & adjusted by reading the Stack Highwater
      ,
      NULL, 3 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      NULL);

  xTaskCreate(
      TaskIOT, "IOT" // A name just for humans
      ,
      256 // This stack size can be checked & adjusted by reading the Stack Highwater
      ,
      NULL, 1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      &TaskIOTHandle);
}

void loop()
{
}

void TaskTrigger(void *pvParameters __attribute__((unused)))
{
  // initialize in pin
  for (auto &item : triggers)
  {
    item.setup();
  }

  for (;;)
  {
    for (auto &item : triggers)
    {
      item.loop();
    }

    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

void TaskIOT(void *pvParameters __attribute__((unused)))
{
  Ethernet.begin(ETH_MAC, ETH_IP, ETH_GATEWAY, ETH_SUBNET);
  vTaskDelay(1);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    vTaskDelete(TaskIOTHandle); // Ethernet shield not found, delete task
  }

  while (Ethernet.linkStatus() == LinkOFF)
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("ERROR: No ethernet cable connected, wait");
      xSemaphoreGive(xSerialSemaphore);
    }
#endif

    vTaskDelay(60000 / portTICK_PERIOD_MS); // Ethernet cable is not connected. Wait until cable is connected
  }

  staticJsonDocument.clear();
  staticJsonDocument["state"] = MQTT_STATE_ON_PAYLOAD;
  serializeJson(staticJsonDocument, stateOnJson);
  staticJsonDocument["state"] = MQTT_STATE_OFF_PAYLOAD;
  serializeJson(staticJsonDocument, stateOffJson);

  for(auto &item : triggers) {
    item.setup_mqtt();
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqttClient.setCallback(handleMQTTMessage);
  mqttClient.setKeepAlive(30);
  mqttClient.setSocketTimeout(30);

  lastMQTTConnection = xTaskGetTickCount();

  for (;;)
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("INFO: Current client state: ");
      Serial.println(mqttClient.state());
      Serial.println(xTaskGetTickCount());
      /*
      // Possible values for client.state()
      #define MQTT_CONNECTION_TIMEOUT     -4
      #define MQTT_CONNECTION_LOST        -3
      #define MQTT_CONNECT_FAILED         -2
      #define MQTT_DISCONNECTED           -1
      #define MQTT_CONNECTED               0
      #define MQTT_CONNECT_BAD_PROTOCOL    1
      #define MQTT_CONNECT_BAD_CLIENT_ID   2
      #define MQTT_CONNECT_UNAVAILABLE     3
      #define MQTT_CONNECT_BAD_CREDENTIALS 4
      #define MQTT_CONNECT_UNAUTHORIZED    5
      */

     /*
     Return the highwatermart --> on a 8 bit architecture 1 indicate that 1 byte is used
     */
     Serial.println(uxTaskGetStackHighWaterMark(NULL));
      xSemaphoreGive(xSerialSemaphore);
    }
#endif

    handleMQTTConnection();

    mqttClient.loop();
  }
}

void handleMQTTConnection()
{
  if (!mqttClient.connected())
  {
    if ( MQTT_CONNECTION_TIMEOUT < xTaskGetTickCount() - lastMQTTConnection)
    {
      if (mqttClient.connect(MQTT_CLIENT_ID.c_str(), MQTT_USERNAME, MQTT_PASSWORD, MQTT_CONTROLLINO_CONFIG_TOPIC, 0, 1, "dead"))
      {

        mqttClient.publish(MQTT_CONTROLLINO_CONFIG_TOPIC, "alive", true);

        for (auto &item : triggers)
        {
          // MQTT discovery for Home Assistant
          staticJsonDocument.clear();
          staticJsonDocument["cmd_t"] = "~/set";
          staticJsonDocument["stat_t"] = "~/state";
          staticJsonDocument["schema"] = "json";
          staticJsonDocument["~"] = MQTT_LIGHT_PREFIX + item.getId();
          staticJsonDocument["name"] = item.getName();
          staticJsonDocument["unique_id"] = item.getId();
          serializeJson(staticJsonDocument, jsonBuffer);
          publishToMQTT(item.getMqttConfigTopic(), jsonBuffer);
          subscribeToMQTT(item.getMqttCommandTopic());

        }


    lastMQTTConnection = xTaskGetTickCount();
      }
      else
      {
#ifdef DEBUG
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
        {
          Serial.println("ERROR: The connection to the MQTT broker failed");
          xSemaphoreGive(xSerialSemaphore);
        }
#endif
      }
    }
  }
  else
  {
    for (auto &item : triggers)
    {
      if (item.isChanged())
      {
        switch (item.getState())
        {
        case LOW:
          publishToMQTT(item.getMqttStateTopic(), stateOffJson);
          break;
        case HIGH:
          publishToMQTT(item.getMqttStateTopic(), stateOnJson);
          break;
        default:
          break;
        }
      }
    }
  }
}

void publishToMQTT(const char *p_topic, char *p_payload)
{
  if (mqttClient.publish(p_topic, p_payload, true))
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("INFO: MQTT message published successfully, topic & payload: ");
      Serial.println(p_topic);
      Serial.println(p_payload);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
  else
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("ERROR: MQTT message not published, either connection lost, or message too large. Topic & payload: ");
      Serial.println(p_topic);
      Serial.println(p_payload);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }

  return;
}

void subscribeToMQTT(const char *p_topic)
{
  if (mqttClient.subscribe(p_topic))
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("INFO: Sending the MQTT subscribe succeeded for topic: ");
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
  else
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
    {
      Serial.println("ERROR: Sending the MQTT subscribe failed for topic: ");
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
}

void handleMQTTMessage(char *p_topic, byte *p_payload, unsigned int p_length)
{
  // light_R1

  // concatenates the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++)
  {
    payload.concat((char)p_payload[i]);
  }

#ifdef DEBUG
  if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
  {
    Serial.println("INFO: New MQTT message received, topic:");
    Serial.println(p_topic);
    Serial.println("INFO: MQTT payload:");
    Serial.println(payload);

    xSemaphoreGive(xSerialSemaphore);
  }
#endif

  for (auto &item : triggers)
  {
    if (String(item.getMqttCommandTopic()).equals(p_topic))
    {
      staticJsonDocument.clear();
      auto error = deserializeJson(staticJsonDocument, p_payload);
      if (error)
      {

#ifdef DEBUG
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
        {
          Serial.println("deserializeJson() failed with code: ");
          Serial.println(error.c_str());

          xSemaphoreGive(xSerialSemaphore);
        }
#endif
        return;
      }

      if (staticJsonDocument.containsKey("state"))
      {
        if (strcmp(staticJsonDocument["state"], MQTT_STATE_ON_PAYLOAD) == 0)
        {
          item.setState(HIGH);
        }
        else if (strcmp(staticJsonDocument["state"], MQTT_STATE_OFF_PAYLOAD) == 0)
        {
          item.setState(LOW);
        }
        serializeJson(staticJsonDocument, jsonBuffer);

        publishToMQTT(item.getMqttStateTopic(), jsonBuffer);
      }

      break;
    }
  }

  return;
}