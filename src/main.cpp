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
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

#define FLOOR 0

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// FreeRTOS variable and function
void TaskTrigger( void *pvParameters );
void TaskIOT( void *pvParameters );
TaskHandle_t TaskIOTHandle; // handler for TaskIOT

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
long lastMQTTConnection = MQTT_CONNECTION_TIMEOUT;
StaticJsonDocument<256> staticJsonDocument;
char jsonBuffer[256];
String MQTT_CONTROLLINO_PREFIX = "homeassistant/light/controllino_p" + FLOOR;
const char * MQTT_CONTROLLINO_CONFIG_TOPIC = String(MQTT_CONTROLLINO_PREFIX + "/config").c_str();
const char * MQTT_CONTROLLINO_STATE_TOPIC = String(MQTT_CONTROLLINO_PREFIX + "/state").c_str();
const char * MQTT_CONTROLLINO_COMMAND_TOPIC = String(MQTT_CONTROLLINO_PREFIX + "/set").c_str();
const char * MQTT_CONTROLLINO_STATUS_TOPIC = String(MQTT_CONTROLLINO_PREFIX + "/status").c_str();

void connectToMQTT();
void publishToMQTT(const char* p_topic, char* p_payload);
void subscribeToMQTT(const char* p_topic);
void handleMQTTMessage(char* p_topic, byte* p_payload, unsigned int p_length);

#if FLOOR == O
  Trigger triggers[] = {
    Trigger(&button_A0, &light_R0, "Lavanderia", "p0_lavanderia"), // Lavanderia
    Trigger(&button_A2, &light_R1, "Laboratorio", "p0_laboratorio"), // Locale tecnico - Ipotizzato ingresso
    Trigger(&button_A1, &light_R2, "Magazzino", "p0_magazzino"), // Salone
    Trigger(&button_A4, &light_R3, "Garage", "p0_garage"), // Garage
    Trigger(&button_A3, &light_R4, "Ingresso", "p0_ingresso"), // Est. Porta
    Trigger(&button_A5, &light_R5, "Gradini", "p0_gradini") // Gradini
  }; // Luci esterne su A3
#elif FLOOR == 1

#endif

// Trigger map
/*
Trigger triggers[] = {
  Trigger(&button_A5, &light_R0), // Ballatoio
  Trigger(&button_A14, &light_R1), // Ext nord
  Trigger(&button_A13, &light_R2), // Ext cucina
  Trigger(&button_A4, &light_R3), // Stanza 2 -- Ema
  Trigger(&button_A12, &light_R4), // Cucina
  Trigger(&button_A8, &light_R5), // Stanza 3 -- Gio
  Trigger(&button_A2, &light_R6), // Corridoio
  Trigger(&button_A9, &light_R5), // Esterno 3 -- Gio --> default R7
  Trigger(&button_A6, &light_R8), // Lavanderia
  Trigger(&button_A7, &light_R9), // Bagno piccolo
  Trigger(&button_A11, &light_R10), // Sala
  Trigger(&button_A3, &light_R3), // Esterno 2 - Ema --> default R11
  Trigger(&button_A1, &light_R12), // Esterno 1
  Trigger(&button_A10, &light_R13), // Bagno
  Trigger(&button_A0, &light_R14) // Stanza 1
};
*/


void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }


   xTaskCreate(
    TaskTrigger
    ,  "Trigger"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    TaskIOT
    ,  "IOT"  // A name just for humans
    ,  2048  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &TaskIOTHandle );
}

void loop() {
}

void TaskTrigger( void *pvParameters __attribute__((unused)) ) {
  // initialize in pin
  for(auto &item : triggers) {
    item.setup();
  }

  for (;;) {
    for(auto &item : triggers) {
      item.loop();
    }

    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

void TaskIOT( void *pvParameters __attribute__((unused)) ) {
  
  Ethernet.begin(ETH_MAC, ETH_IP, ETH_GATEWAY, ETH_SUBNET);
  vTaskDelay(1);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    vTaskDelete(TaskIOTHandle); // Ethernet shield not found, delete task
  }
  
  while (Ethernet.linkStatus() == LinkOFF) {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("ERROR: No ethernet cable connected, wait");
      xSemaphoreGive( xSerialSemaphore );
    }

    vTaskDelay(60000 / portTICK_PERIOD_MS); // Ethernet cable is not connected. Wait until cable is connected
  }

  for(auto &item : triggers) {
    item.setup_mqtt();
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqttClient.setCallback(handleMQTTMessage);

  for (;;) {
    connectToMQTT();
    mqttClient.loop();
  }
}


void connectToMQTT() {
  if (!mqttClient.connected()) {
    if (lastMQTTConnection + MQTT_CONNECTION_TIMEOUT < xTaskGetTickCount()) {
      if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_CONTROLLINO_CONFIG_TOPIC, 0, 1, "dead")) {

        mqttClient.publish(MQTT_CONTROLLINO_CONFIG_TOPIC, "alive", true);

        staticJsonDocument["cmd_t"] = "~/set";
        staticJsonDocument["stat_t"] = "~/state";
        staticJsonDocument["schema"] = "json";

        for(auto &item : triggers) {
          // MQTT discovery for Home Assistant
          staticJsonDocument["~"] = item.getMqttPrefix();
          staticJsonDocument["name"] = item.getName();
          staticJsonDocument["unique_id"] = item.getId();
          serializeJson(staticJsonDocument, jsonBuffer);
          publishToMQTT(item.getMqttCongigTopic(), jsonBuffer);

          subscribeToMQTT(item.getMqttCommandTopic());
        } 
      }
      else {
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
        {
          Serial.println("ERROR: The connection to the MQTT broker failed");
          xSemaphoreGive( xSerialSemaphore );
        }
      }
      lastMQTTConnection = xTaskGetTickCount();
    }
  }
}

void publishToMQTT(const char* p_topic, char* p_payload) {
  if (mqttClient.publish(p_topic, p_payload, true)) {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("INFO: MQTT message published successfully, topic & payload: ");
      Serial.println(p_topic);
      Serial.println(p_payload);
      xSemaphoreGive( xSerialSemaphore );
    }
  } else {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("ERROR: MQTT message not published, either connection lost, or message too large. Topic & payload: ");
      Serial.println(p_topic);
      Serial.println(p_payload);
      xSemaphoreGive( xSerialSemaphore );
    }
  }

  return;
}

void subscribeToMQTT(const char* p_topic) {
  if (mqttClient.subscribe(p_topic)) {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("INFO: Sending the MQTT subscribe succeeded for topic: ");
      Serial.println(p_topic);
      xSemaphoreGive( xSerialSemaphore );
    }
  } else {
   if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("ERROR: Sending the MQTT subscribe failed for topic: ");
      Serial.println(p_topic);
      xSemaphoreGive( xSerialSemaphore );
    }
  }
}

void handleMQTTMessage(char* p_topic, byte* p_payload, unsigned int p_length) {
  // light_R1

  // concatenates the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
  {
    Serial.println("INFO: New MQTT message received, topic:");
    Serial.println(p_topic);
    Serial.println("INFO: MQTT payload:");
    Serial.println(payload);

    xSemaphoreGive( xSerialSemaphore );
  }

  for(auto &item : triggers) {
    if(String(item.getMqttCommandTopic()).equals(p_topic)) {
      DynamicJsonDocument doc(1024);
      auto error = deserializeJson(doc, p_payload);
      if (error) {
        if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
        {
          Serial.println("deserializeJson() failed with code: ");
          Serial.println(error.c_str());

          xSemaphoreGive( xSerialSemaphore );
        }
        return;
      }

      if (doc.containsKey("state")) {
        if (strcmp(doc["state"], MQTT_STATE_ON_PAYLOAD) == 0) {
          item.setState(HIGH);
        } else if (strcmp(doc["state"], MQTT_STATE_OFF_PAYLOAD) == 0) {
          item.setState(LOW);
        }
        serializeJson(doc, jsonBuffer);

        publishToMQTT(item.getMqttStateopic(), jsonBuffer);
      }

      break;
    }
  }

  return;
}