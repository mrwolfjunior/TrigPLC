#include <Light.h>

Light::Light(int pin) {
	this->pin = pin;
	state = LOW;
}

void Light::setup(void) {
	// init hardware
    pinMode(pin, OUTPUT);
    return;
}

void Light::setState(int newState) {
	state = newState;
	digitalWrite(pin, state);
	return;
}

int Light::triggerLight(void) {
	setState(!state);
	return state;
}

int Light::getState(void) {
	return state;
}