#include <Button.h>
#include <Light.h>
#include <Trigger.h>
#include <Controllino.h>

Button button_A0 (CONTROLLINO_A0);
Button button_A1 (CONTROLLINO_A1);
Button button_A3 (CONTROLLINO_A3);
Button button_A2 (CONTROLLINO_A2);
Button button_A4 (CONTROLLINO_A4);
Button button_A5 (CONTROLLINO_A5);
Button button_A6 (CONTROLLINO_A6);
Button button_A7 (CONTROLLINO_A7);
Button button_A8 (CONTROLLINO_A8);
Button button_A9 (CONTROLLINO_A9);
Button button_A10 (CONTROLLINO_A10);
Button button_A11 (CONTROLLINO_A11);
Button button_A12 (CONTROLLINO_A12);
Button button_A13 (CONTROLLINO_A13);
Button button_A14 (CONTROLLINO_A14);

Light light_R0(CONTROLLINO_R0);
Light light_R1(CONTROLLINO_R1);
Light light_R2(CONTROLLINO_R2);
Light light_R3(CONTROLLINO_R3);
Light light_R4(CONTROLLINO_R4);
Light light_R5(CONTROLLINO_R5);
Light light_R6(CONTROLLINO_R6);
Light light_R7(CONTROLLINO_R7);
Light light_R8(CONTROLLINO_R8);
Light light_R9(CONTROLLINO_R9);
Light light_R10(CONTROLLINO_R10);
Light light_R11(CONTROLLINO_R11);
Light light_R12(CONTROLLINO_R12);
Light light_R13(CONTROLLINO_R13);
Light light_R14(CONTROLLINO_R14);

// Ethernet variable
byte ETH_MAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ETH_IP[] = { 192, 168, 88, 15 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte ETH_GATEWAY[] = { 192, 168, 88, 1 };                   // internet access via router
byte ETH_SUBNET[] = { 255, 255, 255, 0 };                  //subnet mask

#define MQTT_USERNAME     "mqtt_user"
#define MQTT_PASSWORD     "mqtt_password"
#define MQTT_SERVER       "192.168.88.232"
#define MQTT_SERVER_PORT  1883
#define MQTT_CONNECTION_TIMEOUT 5000

#define MQTT_STATE_ON_PAYLOAD   "ON"
#define MQTT_STATE_OFF_PAYLOAD  "OFF"

char MQTT_CLIENT_ID[7] = {0};
