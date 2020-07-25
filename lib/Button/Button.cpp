#include <Button.h>

Button::Button(int in, int out) {
    inPin = in;
    outPin = out;
    lightState = LOW;
    lastDebounceTime = 0;
};

int Button::getInPin(void) {
    return inPin;
};
int Button::getOutPin(void) {
    return outPin;
};

int Button::getCurrentState(void) {
    return currentState;
}

bool Button::isPressed(void) {
	if(previousSteadyState == LOW && lastSteadyState == HIGH)
		return true;
	else
		return false;
}

bool Button::isReleased(void) {
	if(previousSteadyState == HIGH && lastSteadyState == LOW)
		return true;
	else
		return false;
}

void Button::triggerLight(void) {
	if(previousSteadyState == HIGH && lastSteadyState == LOW) {
		lightState =! lightState;
		digitalWrite(outPin, lightState);
	}
    return;
}

void Button::setup(void) {
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);

    // Init all variables
    currentState = digitalRead(inPin);
    previousSteadyState = digitalRead(inPin);
	lastSteadyState = digitalRead(inPin);
	lastFlickerableState = digitalRead(inPin);

    digitalWrite(outPin, lightState);       // Make sure light is off

    return;
}

void Button::setup(int time) {
    this->setup();
    debounceTime = time;
    return;
}

int Button::loop(void) {

	currentState = digitalRead(inPin);

	if (currentState != lastFlickerableState) {

		lastDebounceTime = millis();
		lastFlickerableState = currentState;
	}

	if ((millis() - lastDebounceTime) >= debounceTime) {
		previousSteadyState = lastSteadyState;
		lastSteadyState = currentState;
	}
}