#ifndef Trigger_h
#define Trigger_h

#include <Arduino.h>
#include <Light.h>
#include <Button.h>
#include <ArduinoJson.h>

class Trigger
{
	private:
                Button * button;
                Light * light;
                bool isChangedFlag;
                String name;
                String id;
        public:
                Trigger(Button * button, Light * light, String name, String id);
                void setup(void);
                void loop(void);
                void setState(int newState);
                int getState(void);
                bool isChanged(void);
                String getName(void);
                String getId(void);
};

#endif
