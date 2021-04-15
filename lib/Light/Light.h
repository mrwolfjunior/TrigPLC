#ifndef Light_h
#define Light_h

#include <Arduino.h>

class Light
{
	private:
                uint8_t pin;
                int state;
        public:
                Light(int pin);
                void setup(void);
                void setState(int newState);
                int triggerLight(void);
                int getState(void);
};

#endif
