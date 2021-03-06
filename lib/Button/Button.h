#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
	private:
                int pin;
                int currentState;
                int lastState;
                unsigned long lastCheck;
        public:
                Button(int pin);
                void setup(void);
                boolean isPressed(void);
};

#endif
