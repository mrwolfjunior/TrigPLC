#include <Button.h>

Button::Button(int pin) {
	this->pin = pin;
}

void Button::setup(void) {
	// init hardware
    pinMode(pin, INPUT);

    // init private variables
    lastState = digitalRead(pin);
    currentState = lastState;
    return;
}

boolean Button::isPressed(void) {
	lastState = currentState;
	currentState = digitalRead(pin);

	if(currentState == LOW && lastState == HIGH) {
		return true;
	}
	
	return false;
}