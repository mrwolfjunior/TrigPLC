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
                String MQTT_PREFIX;
                String MQTT_CONFIG_TOPIC;
                String MQTT_STATE_TOPIC;
                String MQTT_COMMAND_TOPIC;
                String MQTT_STATUS_TOPIC;
        public:
                /**
                 * @brief Construct a new Trigger object
                 * 
                 * @param button Button object
                 * @param light Light object
                 * @param name Friendly name
                 * @param id unique id for MQTT
                 */
                Trigger(Button * button, Light * light, String name, String id);
                /**
                 * @brief Function to be called in the setup function
                 * 
                 */
                void setup(void);
                /**
                 * @brief Function to be called to initialize the mqtt variable
                 * 
                 */
                void setup_mqtt(void);
                /**
                 * @brief Function to be called in the loop function
                 * 
                 */
                void loop(void);
                /**
                 * @brief Set the light state
                 * 
                 * @param newState HIGH or LOW
                 */
                void setState(int newState);
                /**
                 * @brief Get the current light state
                 * 
                 * @return int HIGH or LOW
                 */
                int getState(void);
                /**
                 * @brief Function used to monitor the state change related to MQTT
                 * 
                 * @return bool
                 */
                bool isChanged(void);
                /**
                 * @brief Get the friendly name
                 * 
                 * @return String 
                 */
                String getName(void);
                /**
                 * @brief Get the unique id
                 * 
                 * @return String 
                 */
                String getId(void);
                /**
                 * @brief Get the Mqtt Prefix
                 * 
                 * @return const char* 
                 */
                const char * getMqttPrefix(void);
                /**
                 * @brief Get the Mqtt Config Topic
                 * 
                 * @return const char* 
                 */
                const char * getMqttConfigTopic(void);
                /**
                 * @brief Get the Mqtt State Topic
                 * 
                 * @return const char* 
                 */
                const char * getMqttStateTopic(void);
                /**
                 * @brief Get the Mqtt Command Topic 
                 * 
                 * @return const char* 
                 */
                const char * getMqttCommandTopic(void);
};

#endif
