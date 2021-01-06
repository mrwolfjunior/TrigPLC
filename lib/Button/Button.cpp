#include <Button.h>
#define ETIME 50
#define BLOCKPRESS 1000 // 1 sec of block
#define PRECISION 0.85

Button::Button(int in, int out) {
	inPin = in;
    outPin = out;
    lightState = LOW;
	lastPressTime = millis();
	eStartTime = lastPressTime;
	// inizialize with 0 the array
	counter = new int[2]();
}

void Button::setup(void) {
	// init hardware
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);
	digitalWrite(outPin, lightState); // Make sure light is off

    // init private variables
    currentState = digitalRead(inPin);
    lastState = currentState;
    
    return;
}

void resetCounter(int * counter) {
	counter[0] = 0;
	counter[1] = 0;
	return;
}

void Button::loop(void) {
	currentState = digitalRead(inPin);

	if(isEvaluating == false) {
		if(lastState == LOW && currentState == HIGH && (millis() - lastPressTime > BLOCKPRESS)) { // if the button is pressed and is not evaluating start the evaluation time
			isEvaluating = true;
			eStartTime = millis();
			resetCounter(counter);
		}
	}
	else { // is evaluating
		
		if(millis() - eStartTime > ETIME) { // finished evaluation time	
			isEvaluating = false;
			// update last press
			lastPressTime = millis();
			
			// check percentage of success
			if((counter[1] / (counter[1] + counter[0])) > PRECISION) {
				lightState = !lightState;
				digitalWrite(outPin, lightState);
			}
		}
		else {
			// is evaluating, update the counter
			if(currentState == LOW) 
				++counter[0];
			else
				++counter[1];
		}
	}

	lastState = currentState;
	return;
}