#include <Trigger.h>

Trigger::Trigger(Button * button, Light * light, String name, String id) {
	this->button = button;
	this->light = light;
	this->name = name;
	this->id = id;
	isChangedFlag = false;
	return;
}

void Trigger::setup(void) {
	button->setup();
	light->setup();
	return;
}

void Trigger::loop(void) {
	if(button->isPressed() == true) {
		light->triggerLight();
		isChangedFlag = true;
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