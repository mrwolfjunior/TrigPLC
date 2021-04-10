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
                char jsonConfig[256];
                StaticJsonDocument<256> staticJsonDocument;
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
                int getState(void);
                bool isChanged(void);
                String getName(void);
                String getId(void);
                char * getJsonConfig(void);
                const char * getMqttPrefix(void);
                const char * getMqttConfigTopic(void);
                const char * getMqttStateopic(void);
                const char * getMqttCommandTopic(void);
};

#endif
