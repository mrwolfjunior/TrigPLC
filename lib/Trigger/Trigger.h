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
                String id;
                String MQTT_PREFIX;
                String MQTT_CONFIG_TOPIC;
                String MQTT_STATE_TOPIC;
                String MQTT_COMMAND_TOPIC;
                String MQTT_STATUS_TOPIC;
        public:
                Trigger(Button * button, Light * light, String name, String id);
                void setup(void);
                void setup_mqtt(void);
                void loop(void);
                void setState(int newState);
                String getName(void);
                String getId(void);
                const char * getMqttPrefix(void);
                const char * getMqttCongigTopic(void);
                const char * getMqttStateopic(void);
                const char * getMqttCommandTopic(void);
};

#endif
