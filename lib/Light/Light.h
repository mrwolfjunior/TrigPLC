#ifndef Light_h
#define Light_h

#include <Arduino.h>

class Light
{
	private:
                int pin;
                int state;
        public:
                Light(int pin);
                void setup(void);
                void setState(int newState);
                int triggerLight(void);
};

#endif
