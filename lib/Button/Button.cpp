#include <Button.h>
#define BTIME 15

Button::Button(int pin) {
	this->pin = pin;
}

void Button::setup(void) {
	// init hardware
    pinMode(pin, INPUT);

    // init private variables
    lastState = digitalRead(pin);
    currentState = lastState;
	lastCheck = millis();
    return;
}

boolean Button::isPressed(void) {
	if(millis() - lastCheck > BTIME) { // Delay between reads for stability
		lastState = currentState;
		currentState = digitalRead(pin);
		lastCheck = millis();

		if(currentState == LOW && lastState == HIGH)
			return true;
	}
	
	return false;
}