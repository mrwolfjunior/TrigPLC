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

#include <Arduino.h>
#include <Controllino.h>
#include <Button.h>

#define DEBOUNCE_TIME 50  // ms

Button buttons[] = {
  Button(CONTROLLINO_R0, CONTROLLINO_A5), // Ballatoio
  Button(CONTROLLINO_R1, CONTROLLINO_A14), // Ext nord
  Button(CONTROLLINO_R2, CONTROLLINO_A13), // Ext cucina
  Button(CONTROLLINO_R3, CONTROLLINO_A4), // Stanza 2 -- Ema
  Button(CONTROLLINO_R4, CONTROLLINO_A12), // Cucina
  Button(CONTROLLINO_R5, CONTROLLINO_A8), // Stanza 3 -- Gio
  Button(CONTROLLINO_R6, CONTROLLINO_A2), // Corridoio
  Button(CONTROLLINO_R7, CONTROLLINO_A9), // Esterno 3
  Button(CONTROLLINO_R8, CONTROLLINO_A6), // Lavanderia
  Button(CONTROLLINO_R9, CONTROLLINO_A7), // Bagno piccolo
  Button(CONTROLLINO_R10, CONTROLLINO_A11), // Sala
  Button(CONTROLLINO_R11, CONTROLLINO_A3), // Esterno 2
  Button(CONTROLLINO_R12, CONTROLLINO_A1), // Esterno 1
  Button(CONTROLLINO_R13, CONTROLLINO_A10), // Bagno
  Button(CONTROLLINO_R14, CONTROLLINO_A0) // Stanza 1
};

void setup() {
  Serial.begin(57600);

  for(auto &item : buttons) {
    // initialize in/out pin
    item.setup(DEBOUNCE_TIME);
  }
}

void loop() {
  for(auto &item : buttons) {
    item.loop();
  }

  for(auto &item : buttons) {
    item.triggerLight();
  }
}