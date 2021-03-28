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
#include <config.h>

// FreeRTOS variable and function
void TaskTrigger( void *pvParameters );
void TaskIOT( void *pvParameters );
TaskHandle_t TaskIOTHandle; // handler for TaskIOT

// Ethernet variable
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 88, 15 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 88, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port 
String readString;

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
  Trigger(&button_A0, &light_R0), // Cucina
  Trigger(&button_A2, &light_R1), // Locale tecnico - Ipotizzato ingresso
  Trigger(&button_A1, &light_R2), // Salone
  Trigger(&button_A4, &light_R3), // Garage
  Trigger(&button_A3, &light_R4), // Est. Porta
  Trigger(&button_A5, &light_R5) // Gradini
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
  
  Ethernet.begin(mac, ip, gateway, subnet);
  vTaskDelay(1);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    vTaskDelete(TaskIOTHandle); // Ethernet shield not found, delete task
  }
  
  while (Ethernet.linkStatus() == LinkOFF) {
    vTaskDelay(60000 / portTICK_PERIOD_MS); // Ethernet cable is not connected. Wait until cable is connected
  }

  server.begin(); // start to listen for clients

  for (;;) {
    EthernetClient client = server.available();
    if (client) {
      while (client.connected()) {   
        if (client.available()) {
          char c = client.read();
      
          //read char by char HTTP request
          if (readString.length() < 100) {
            //store characters to string
            readString += c;
          }

          //if HTTP request has ended
          if (c == '\n') {          
            //stopping client
            client.stop();
            //controls the Arduino if you press the buttons
            if (readString.indexOf("?LabOn") >0){
              light_R1.setState(HIGH);
            }
            if (readString.indexOf("?LabOff") >0){
              light_R1.setState(LOW);
            }
            //clearing string for next read
            readString="";  
            
          }
        }
      }
    }
  }
}