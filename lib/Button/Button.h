#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
	private:
                uint8_t pin;
                int currentState;
                int lastState;
        public:
                Button(int pin);
                void setup(void);
                boolean isPressed(void);
};

#endif
