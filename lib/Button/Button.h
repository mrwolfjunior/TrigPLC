#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
	private:
                int pin;
                int currentState;
                int lastState;
                int countLow;
                int countHigh;
                float percentage;
                boolean isEvaluating = false;
                boolean midEvaluation = false;
                // evaluation starting time
                unsigned long eStartTime;
                unsigned long lastPressTime;
                void resetCounter(void);
        public:
                Button(int pin);
                void setup(void);
                boolean isPressed(void);
};

#endif
