#include <Arduino_FreeRTOS.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <config.h>
#include <semphr.h>
#include <avr/wdt.h>

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// FreeRTOS variable and function
void TaskTrigger(void *pvParameters);
void TaskIOT(void *pvParameters);
TaskHandle_t TaskIOTHandle; // handler for TaskIOT

String discoveryPrefix = MQTT_DISCOVERY_PREFIX;

EthernetClient ethClient;

PubSubClient mqttClient(ethClient);
long lastMQTTConnection;
StaticJsonDocument<640> staticJsonDocument;
char jsonBuffer[400];
char stateOnJson[64];
char stateOffJson[64];

void handleMQTTConnection();
void publishToMQTT(const char *p_topic, char *p_payload, bool retain = false);
void subscribeToMQTT(const char *p_topic);
void handleMQTTMessage(char *p_topic, byte *p_payload, unsigned int p_length);

void setup()
{
  // Enable Watchdog Timer
  wdt_enable(WDTO_8S);

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  while (!Serial)
  {
    ; // wait for serial port to connect.
  }

  if (xSerialSemaphore == NULL) 
  {
    xSerialSemaphore = xSemaphoreCreateMutex(); 
    if ((xSerialSemaphore) != NULL)
      xSemaphoreGive((xSerialSemaphore)); 
  }

  xTaskCreate(
      TaskTrigger, "Trigger"
      ,
      128 
      ,
      NULL, 3 
      ,
      NULL);

  xTaskCreate(
      TaskIOT, "IOT"
      ,
      512 
      ,
      NULL, 1 
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
    wdt_reset();

    // Scan all physical buttons once per loop
    for (auto button : all_buttons)
    {
      if (button) button->update();
    }

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
    wdt_reset();
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.println(F("ERROR: No ethernet cable connected, wait"));
      xSemaphoreGive(xSerialSemaphore);
    }
#endif

    vTaskDelay(60000 / portTICK_PERIOD_MS); 
  }

  staticJsonDocument.clear();
  staticJsonDocument["state"] = MQTT_STATE_ON_PAYLOAD;
  serializeJson(staticJsonDocument, stateOnJson);
  staticJsonDocument.clear();
  staticJsonDocument["state"] = MQTT_STATE_OFF_PAYLOAD;
  serializeJson(staticJsonDocument, stateOffJson);

  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqttClient.setCallback(handleMQTTMessage);
  mqttClient.setKeepAlive(30);
  mqttClient.setSocketTimeout(30);

  lastMQTTConnection = xTaskGetTickCount();

  for (;;)
  {
    wdt_reset();
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.print(F("INFO: MQTT State: "));
      Serial.print(mqttClient.state());
      Serial.print(F(" | Stack: "));
      Serial.println(uxTaskGetStackHighWaterMark(NULL));
      xSemaphoreGive(xSerialSemaphore);
    }
#endif

    handleMQTTConnection();

    mqttClient.loop();
    vTaskDelay(1);
  }
}

void handleMQTTConnection()
{
  if (!mqttClient.connected())
  {
    if (MQTT_BROKER_TIMEOUT < xTaskGetTickCount() - lastMQTTConnection)
    {
      if (mqttClient.connect(MQTT_CLIENT_ID.c_str(), MQTT_USERNAME, MQTT_PASSWORD, MQTT_CONTROLLINO_CONFIG_TOPIC, 0, 1, "dead"))
      {
        publishToMQTT(MQTT_CONTROLLINO_CONFIG_TOPIC, (char*)"alive", true);

        for (auto &item : triggers)
        {
          if (item.getName().length() != 0)
          {
            // MQTT discovery for Home Assistant
            staticJsonDocument.clear();
            staticJsonDocument["schema"] = "json";
            staticJsonDocument["command_topic"] = "~/set";
            staticJsonDocument["state_topic"] = "~/state";
            staticJsonDocument["~"] = item.getMqttPrefix();
            staticJsonDocument["name"] = item.getName();
            
            // Unique ID must be globally unique across HA
            String uid = MQTT_CLIENT_ID + "_" + item.getId();
            staticJsonDocument["unique_id"] = uid;
            
            // Device grouping (Full keys for better compatibility)
            JsonObject device = staticJsonDocument.createNestedObject("device");
            JsonArray identifiers = device.createNestedArray("identifiers");
            identifiers.add(MQTT_CLIENT_ID);
            device["name"] = MQTT_DEVICE_NAME;
            device["model"] = "Controllino Mega";
            device["sw_version"] = "1.1.0";

            serializeJson(staticJsonDocument, jsonBuffer);
            publishToMQTT(item.getMqttConfigTopic(), jsonBuffer, true);
            subscribeToMQTT(item.getMqttCommandTopic());
          }
        }

        lastMQTTConnection = xTaskGetTickCount();
      }
      else
      {
#ifdef DEBUG
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
        {
          Serial.println(F("ERROR: MQTT connection failed"));
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

void publishToMQTT(const char *p_topic, char *p_payload, bool retain)
{
  if (mqttClient.publish(p_topic, p_payload, retain))
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.print(F("INFO: Published "));
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
  else
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.print(F("ERROR: Publish failed "));
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
}

void subscribeToMQTT(const char *p_topic)
{
  if (mqttClient.subscribe(p_topic))
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.print(F("INFO: Subscribed "));
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
  else
  {
#ifdef DEBUG
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
    {
      Serial.print(F("ERROR: Subscribing failed "));
      Serial.println(p_topic);
      xSemaphoreGive(xSerialSemaphore);
    }
#endif
  }
}

void handleMQTTMessage(char *p_topic, byte *p_payload, unsigned int p_length)
{
#ifdef DEBUG
  if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
  {
    Serial.print(F("INFO: MQTT Message on "));
    Serial.println(p_topic);
    xSemaphoreGive(xSerialSemaphore);
  }
#endif

  for (auto &item : triggers)
  {
    if (strcmp(item.getMqttCommandTopic(), p_topic) == 0)
    {
      staticJsonDocument.clear();
      auto error = deserializeJson(staticJsonDocument, p_payload, p_length);
      if (error)
      {
#ifdef DEBUG
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)100) == pdTRUE)
        {
          Serial.print(F("ERROR: JSON parse failed: "));
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
}