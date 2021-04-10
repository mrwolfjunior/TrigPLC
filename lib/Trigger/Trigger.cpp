#include <Trigger.h>

Trigger::Trigger(Button * button, Light * light, String name) {
	this->button = button;
	this->light = light;
	this->name = name;
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
	}

	return;
}