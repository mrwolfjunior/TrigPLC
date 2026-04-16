#include <Trigger.h>

static char topicBuffer[64];

Trigger::Trigger(Button ** buttons, uint8_t buttonCount, Light * light, String name, String id, String type) {
	this->buttons = buttons;
    this->buttonCount = buttonCount;
	this->light = light;
	this->name = name;
	this->id = id;
	this->type = type;
	isChangedFlag = false;
}

Trigger::Trigger(Button * button, Light * light, String name, String id, String type) {
    // For single button triggers, we create a small array or just handle it.
    // To save RAM, we reuse the pointer logic. 
    // WARNING: This assumes the button pointer passed is stored in a way that allows address-of.
    // Better: create a static array of size 1 for these cases in the generator.
    this->buttons = new Button*[1];
    this->buttons[0] = button;
    this->buttonCount = 1;
	this->light = light;
	this->name = name;
	this->id = id;
	this->type = type;
	isChangedFlag = false;
}

void Trigger::setup(void) {
	for(uint8_t i=0; i<buttonCount; i++) {
        if(buttons[i]) buttons[i]->setup();
    }
	light->setup();
}

void Trigger::loop(void) {
	for(uint8_t i=0; i<buttonCount; i++) {
        if(buttons[i] && buttons[i]->wasPressed()) {
            light->triggerLight();
            isChangedFlag = true;
            break; // One button press is enough
        }
    }
}

void Trigger::setState(int newState) {
	light->setState(newState);
}

int Trigger::getState(void) {
	return light->getState();
}

bool Trigger::isChanged(void) {
	if(isChangedFlag) {
		isChangedFlag = false;
		return true;
	}
	return false;
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