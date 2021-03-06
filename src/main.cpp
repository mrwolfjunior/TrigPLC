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
#include <config.h>
#include <Trigger.h>

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

void TaskTrigger( void *pvParameters );

/*
// Luci esterne su A3

Button buttons[] = {
  Button(CONTROLLINO_A0, CONTROLLINO_R0), // Cucina
  Button(CONTROLLINO_A2, CONTROLLINO_R1), // Locale tecnico - Ipotizzato ingresso
  Button(CONTROLLINO_A1, CONTROLLINO_R2), // Salone
  Button(CONTROLLINO_A4, CONTROLLINO_R3), // Garage
  Button(CONTROLLINO_A3, CONTROLLINO_R4), // Est. Porta
  Button(CONTROLLINO_A5, CONTROLLINO_R5) // Gradini
};
*/

void setup() {
  Serial.begin(9600);

   xTaskCreate(
    TaskTrigger
    ,  "Trigger"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );


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
  }
}