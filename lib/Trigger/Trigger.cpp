#include <Trigger.h>
#include <Logger.h>

Trigger::Trigger(Button * button, Light * light) {
	this->button = button;
	this->light = light;
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