#ifndef Trigger_h
#define Trigger_h

#include <Arduino.h>
#include <Light.h>
#include <Button.h>

class Trigger
{
	private:
                Button * button;
                Light * light;
                String name;
        public:
                Trigger(Button * button, Light * light, String name);
                void setup(void);
                void loop(void);
};

#endif
