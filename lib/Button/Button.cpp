#include <Button.h>
#define PRESSTIME 500

Button::Button(int in, int out){
    inPin = in;
    outPin = out;
    lightState = LOW;
    lastDebounceTime = 0;
};

// Constructor delegation
Button::Button(int in, int out1, int out2):Button(in, out1) {
	outPin2 = out2;
	lightCount = 0;
	lastPress = 0;
}

int Button::getInPin(void) {
    return inPin;
};
int Button::getOutPin(void) {
    return outPin;
};

int Button::getLightState(void) {
    return lightState;
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
	if (outPin2 == NOTUSED) {
		if(previousSteadyState == HIGH && lastSteadyState == LOW) {
			lightState =! lightState;
			digitalWrite(outPin, lightState);
		}
	}
	else {
		if(previousSteadyState == HIGH && lastSteadyState == LOW) {
			if(lightCount == 0)
				lastPress = millis();
			lightCount++;
		}

		if(millis() - lastPress > PRESSTIME && lightCount >= 1) { // wait for evaluation time
				if(lightCount == 1) { // single press
					lightState =! lightState;
					digitalWrite(outPin, lightState);
				}
				else { // double press
					if(lightState == HIGH) { // If primary light is on
						if(digitalRead(outPin2) == HIGH ) { // Both on, power off
							lightState =! lightState;
							digitalWrite(outPin, lightState);
							digitalWrite(outPin2, LOW);
						}
						else { // Power on only second, first already on
							digitalWrite(outPin2, HIGH);
						}
					}
					else { // If primary is off
						if(digitalRead(outPin2) == LOW) { // Both off, power on
							lightState =! lightState;
							digitalWrite(outPin, lightState);
							digitalWrite(outPin2, HIGH);
						}
						else { // Power off only second, first already off
							digitalWrite(outPin2, LOW);
						}
					}
				}
				lightCount = 0;
			}
	}

    return;
}

void Button::setup(void) {
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);
	if (outPin2 != NOTUSED) {
		pinMode(outPin2, OUTPUT);
		// Not necessary to have the digitalWrite
	}

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