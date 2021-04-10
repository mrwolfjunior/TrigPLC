#include <Trigger.h>

Trigger::Trigger(Button * button, Light * light, String name, String id) {
	this->button = button;
	this->light = light;
	this->name = name;
	this->id = id;
	MQTT_PREFIX = "homeassistant/light/" + id;
	return;
}

void Trigger::setup(void) {
	button->setup();
	light->setup();
	return;
}

void Trigger::setup_mqtt(void) {
	MQTT_CONFIG_TOPIC = MQTT_PREFIX + "/config";
	MQTT_STATE_TOPIC = MQTT_PREFIX + "/state";
	MQTT_COMMAND_TOPIC = MQTT_PREFIX + "/set";
	MQTT_STATUS_TOPIC = MQTT_PREFIX + "/status";
	return;
}

void Trigger::loop(void) {
	if(button->isPressed() == true) {
		light->triggerLight();
	}

	return;
}

void Trigger::setState(int newState) {
	return light->setState(newState);
}

String Trigger::getName(void) {
	return name;
}
String Trigger::getId(void) {
	return id;
}

const char * Trigger::getMqttPrefix(void) {
	return MQTT_PREFIX.c_str();
}

const char * Trigger::getMqttCongigTopic(void) {
	return MQTT_CONFIG_TOPIC.c_str();
}

const char * Trigger::getMqttStateopic(void) {
	return MQTT_STATE_TOPIC.c_str();
}

const char * Trigger::getMqttCommandTopic(void) {
	return MQTT_COMMAND_TOPIC.c_str();
}