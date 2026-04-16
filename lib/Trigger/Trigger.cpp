#include <Trigger.h>

static char topicBuffer[64];

Trigger::Trigger(Button * button, Light * light, String name, String id, String type) {
	this->button = button;
	this->light = light;
	this->name = name;
	this->id = id;
	this->type = type;
	isChangedFlag = false;
	return;
}

void Trigger::setup(void) {
	if(button) {
		button->setup();
	}
	light->setup();
	return;
}

void Trigger::loop(void) {
	if(button){
		if(button->isPressed() == true) {
			light->triggerLight();
			isChangedFlag = true;
		}
	}
	return;
}

void Trigger::setState(int newState) {
	return light->setState(newState);
}

int Trigger::getState(void) {
	return light->getState();
}

bool Trigger::isChanged(void) {
	if(isChangedFlag) {
		isChangedFlag = false;
		return true;
	}
	else return false;
}

String Trigger::getName(void) {
	return name;
}
String Trigger::getId(void) {
	return id;
}

String Trigger::getType(void) {
    return type;
}

const char * Trigger::getMqttPrefix(void) {
    snprintf(topicBuffer, sizeof(topicBuffer), "ha/%s/%s", type.c_str(), id.c_str());
	return topicBuffer;
}

const char * Trigger::getMqttConfigTopic(void) {
    snprintf(topicBuffer, sizeof(topicBuffer), "ha/%s/%s/config", type.c_str(), id.c_str());
	return topicBuffer;
}

const char * Trigger::getMqttStateTopic(void) {
    snprintf(topicBuffer, sizeof(topicBuffer), "ha/%s/%s/state", type.c_str(), id.c_str());
	return topicBuffer;
}

const char * Trigger::getMqttCommandTopic(void) {
    snprintf(topicBuffer, sizeof(topicBuffer), "ha/%s/%s/set", type.c_str(), id.c_str());
	return topicBuffer;
}