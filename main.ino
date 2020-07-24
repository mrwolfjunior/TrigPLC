/*
 * Created by Emanuele Bono
 * This code use the ezButton library (customized): https://arduinogetstarted.com/tutorials/arduino-button-library
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-button-toggle-led
 *
 * TODO:
 * - Insert #define DEBUG
 *
 /*
byte inputs[] = {CONTROLLINO_A0, CONTROLLINO_A1, CONTROLLINO_A2, CONTROLLINO_A3, CONTROLLINO_A4,
                 CONTROLLINO_A5, CONTROLLINO_A6, CONTROLLINO_A7, CONTROLLINO_A8, CONTROLLINO_A9,
                 CONTROLLINO_A10, CONTROLLINO_A11, CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14,
                 CONTROLLINO_A15, CONTROLLINO_I16, CONTROLLINO_I17, CONTROLLINO_I18, CONTROLLINO_IN0,
                 CONTROLLINO_IN1};

byte outputs[] = {CONTROLLINO_D0, CONTROLLINO_D1, CONTROLLINO_D2, CONTROLLINO_D3, CONTROLLINO_D4,
                  CONTROLLINO_D5, CONTROLLINO_D6, CONTROLLINO_D7, CONTROLLINO_D8, CONTROLLINO_D9,
                  CONTROLLINO_D10, CONTROLLINO_D11, CONTROLLINO_D12, CONTROLLINO_D13, CONTROLLINO_D14,
                  CONTROLLINO_D15, CONTROLLINO_D16, CONTROLLINO_D17, CONTROLLINO_D18, CONTROLLINO_D19};

byte relays[] = {CONTROLLINO_R0, CONTROLLINO_R1, CONTROLLINO_R2, CONTROLLINO_R3, CONTROLLINO_R4,
                 CONTROLLINO_R5, CONTROLLINO_R6, CONTROLLINO_R7, CONTROLLINO_R8, CONTROLLINO_R9,
                 CONTROLLINO_R10, CONTROLLINO_R11, CONTROLLINO_R12, CONTROLLINO_R13, CONTROLLINO_R14,
                 CONTROLLINO_R15};
*/

#include <Controllino.h>
#include <ezButton.h>     // Modded - Based on ver 1.0.3
#define DEBOUNCE_TIME 50  // ms

ezButton IOMap[] = {
  ezButton(CONTROLLINO_A0, CONTROLLINO_R0, DEBOUNCE_TIME), // Room1
  ezButton(CONTROLLINO_A1, CONTROLLINO_R1, DEBOUNCE_TIME)  // Room2
};

void setup() {
  Serial.begin(57600);   // initialize serial
}

void loop() {

  for(auto &item : IOMap) {
    item.loop();
  }
  
  for(auto &item : IOMap) {
    item.triggerLight();
  }
}
