#include <wrapper.h>

//#define DEBUG // uncomment to enable debug

#define MQTT_USERNAME "usr"
#define MQTT_PASSWORD "pwd"
#define MQTT_SERVER "192.168.1.2"
#define MQTT_SERVER_PORT 1883
#define MQTT_STATE_ON_PAYLOAD "ON"
#define MQTT_STATE_OFF_PAYLOAD "OFF"
String MQTT_CLIENT_ID = "cont0";
String MQTT_LIGHT_PREFIX = "homeassistant/light/";
String MQTT_CONTROLLINO_PREFIX = "homeassistant/light/" + MQTT_CLIENT_ID;
const char *MQTT_CONTROLLINO_CONFIG_TOPIC = String(MQTT_CONTROLLINO_PREFIX + "/config").c_str();

byte ETH_MAC[6] = { 0x90, 0xA2, 0xDA, 0x7B, 0x5A, 0x18 }; // generated with https://ssl.crox.net/arduinomac/
byte ETH_GATEWAY[] = {192, 168, 1, 1};                // internet access via router
byte ETH_SUBNET[] = {255, 255, 255, 0};               //subnet mask
byte ETH_IP[] = {192, 168, 1, 3};                     // ip in lan (that's what you need to use in your browser. ("192.168.1.178")

/*
Every trigger is composed of:

- button pin (wrapped with button class)
- light pin (wrapped with light class)
- Friendly name
- Unique id

Make sure to use short name and unique id because the JSON buffer is only 256 char
*/
Trigger triggers[] = {
    Trigger(&button_A0, &light_R0, "Room 1", "p0_R1"),   // "Room 1" --> friendly name , "p0_R1" --> unique id
    Trigger(&button_A2, &light_R1, "Room 2", "p0_R2")
}; 
