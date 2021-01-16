#ifndef Button_h
#define Button_h

#include <Arduino.h>
#define NOTUSED -1

class Button
{
	private:
                int inPin;
                int outPin;
                int lightState;
                int currentState;
                int lastState;
                int countLow;
                int countHigh;
                float percentage;
                boolean isEvaluating = false;
                // evaluation starting time
                unsigned long eStartTime;
                unsigned long lastPressTime;
        public:
                Button(int in, int out);
                void resetCounter(void);
                void setup(void);
                void loop(void);
};

#endif
