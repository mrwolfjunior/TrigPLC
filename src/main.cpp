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

// FreeRTOS variable and function
void TaskTrigger( void *pvParameters );
void TaskIOT( void *pvParameters );
TaskHandle_t TaskIOTHandle; // handler for TaskIOT

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
long lastMQTTConnection = MQTT_CONNECTION_TIMEOUT;
StaticJsonDocument<256> staticJsonDocument;
char jsonBuffer[256];
char MQTT_PREFIX[] = "homeassistant/light/laboratorio";
char MQTT_CONFIG_TOPIC[] = "homeassistant/light/laboratorio/config";
char MQTT_STATE_TOPIC[] = "homeassistant/light/laboratorio/state";
char MQTT_COMMAND_TOPIC[] = "homeassistant/light/laboratorio/set";
char MQTT_STATUS_TOPIC[] = "homeassistant/light/laboratorio/status";

void connectToMQTT();
void publishToMQTT(char* p_topic, char* p_payload);
void subscribeToMQTT(char* p_topic);
void handleMQTTMessage(char* p_topic, byte* p_payload, unsigned int p_length);

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


// Luci esterne su A3

Trigger triggers[] = {
  Trigger(&button_A0, &light_R0, "cucina"), // Cucina
  Trigger(&button_A2, &light_R1, "laboratorio"), // Locale tecnico - Ipotizzato ingresso
  Trigger(&button_A1, &light_R2, "magazzino"), // Salone
  Trigger(&button_A4, &light_R3, "garage"), // Garage
  Trigger(&button_A3, &light_R4, "ingresso"), // Est. Porta
  Trigger(&button_A5, &light_R5, "gradini") // Gradini
};


void setup() {



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
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
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
    vTaskDelay(60000 / portTICK_PERIOD_MS); // Ethernet cable is not connected. Wait until cable is connected
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
      if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_STATUS_TOPIC, 0, 1, "dead")) {

        mqttClient.publish(MQTT_STATUS_TOPIC, "alive", true);

        // MQTT discovery for Home Assistant
        staticJsonDocument["~"] = MQTT_PREFIX;
        staticJsonDocument["name"] = "Laboratorio";
        staticJsonDocument["unique_id"] = "laboratorio";
        staticJsonDocument["cmd_t"] = "~/set";
        staticJsonDocument["stat_t"] = "~/state";
        staticJsonDocument["schema"] = "json";
        //staticJsonDocument["state_topic"] = MQTT_STATE_TOPIC;
        //staticJsonDocument["command_topic"] = MQTT_COMMAND_TOPIC;
        serializeJson(staticJsonDocument, jsonBuffer);
        publishToMQTT(MQTT_CONFIG_TOPIC, jsonBuffer);

        subscribeToMQTT(MQTT_COMMAND_TOPIC);
      } else {
        /*
        DEBUG_PRINTLN(F("ERROR: The connection to the MQTT broker failed"));
        DEBUG_PRINT(F("INFO: MQTT username: "));
        DEBUG_PRINTLN(MQTT_USERNAME);
        DEBUG_PRINT(F("INFO: MQTT password: "));
        DEBUG_PRINTLN(MQTT_PASSWORD);
        DEBUG_PRINT(F("INFO: MQTT broker: "));
        DEBUG_PRINTLN(MQTT_SERVER);
        */
      }
        lastMQTTConnection = xTaskGetTickCount();
    }
  }
}

void publishToMQTT(char* p_topic, char* p_payload) {
  if (mqttClient.publish(p_topic, p_payload, true)) {
    /*
    DEBUG_PRINT(F("INFO: MQTT message published successfully, topic: "));
    DEBUG_PRINT(p_topic);
    DEBUG_PRINT(F(", payload: "));
    DEBUG_PRINTLN(p_payload);
    */
  } else {
    /*
    DEBUG_PRINTLN(F("ERROR: MQTT message not published, either connection lost, or message too large. Topic: "));
    DEBUG_PRINT(p_topic);
    DEBUG_PRINT(F(" , payload: "));
    DEBUG_PRINTLN(p_payload);
    */
  }
}

void subscribeToMQTT(char* p_topic) {
  if (mqttClient.subscribe(p_topic)) {
    /*
    DEBUG_PRINT(F("INFO: Sending the MQTT subscribe succeeded for topic: "));
    DEBUG_PRINTLN(p_topic);
    */
  } else {
    /*
    DEBUG_PRINT(F("ERROR: Sending the MQTT subscribe failed for topic: "));
    DEBUG_PRINTLN(p_topic);
    */
  }
}

void handleMQTTMessage(char* p_topic, byte* p_payload, unsigned int p_length) {
  // light_R1

  // concatenates the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  
  /*
    DEBUG_PRINTLN(F("INFO: New MQTT message received"));
    DEBUG_PRINT(F("INFO: MQTT topic: "));
    DEBUG_PRINTLN(p_topic);
    DEBUG_PRINT(F("INFO: MQTT payload: "));
    DEBUG_PRINTLN(payload);
  */
  
  if (String(MQTT_COMMAND_TOPIC).equals(p_topic)) {
    DynamicJsonDocument doc(1024);
    auto error = deserializeJson(doc, p_payload);
    //JsonObject& root = dynamicJsonBuffer.parseObject(p_payload);
    if (error) {
      //Serial.print(F("deserializeJson() failed with code "));
      //Serial.println(error.c_str());
      return;
    }

    if (doc.containsKey("state")) {
      if (strcmp(doc["state"], MQTT_STATE_ON_PAYLOAD) == 0) {
        light_R1.setState(HIGH);
      } else if (strcmp(doc["state"], MQTT_STATE_OFF_PAYLOAD) == 0) {
        light_R1.setState(LOW);
      }
    }
  }
}