#include <Button.h>
#define ETIME 50
#define BLOCKPRESS 1000 // 1 sec of block
#define PRECISION 0.85

void Button::resetCounter(void) {
	countLow = 0;
	countHigh = 0;
	return;
}

Button::Button(int in, int out) {
	inPin = in;
    outPin = out;
    lightState = LOW;
	lastPressTime = millis();
	eStartTime = lastPressTime;
	resetCounter();
}

void Button::setup(void) {
	// init hardware
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);

    // init private variables
    lastState = digitalRead(inPin);
    currentState = lastState;
    
    return;
}

void Button::loop(void) {
	currentState = digitalRead(inPin);
	
	if(isEvaluating) { // is evaluating
		
		if(millis() - eStartTime > ETIME) { // finished evaluation time	
			isEvaluating = false;
			// update last press
			lastPressTime = millis();

			//calculate percentage
			percentage = (float)countHigh / (countHigh + countLow);

			// check percentage of success
			/*
			// for debug
			Serial.print("The countHigh is: ");
			Serial.println(countHigh);
			Serial.print("The countLow is: ");
			Serial.println(countLow);
			Serial.print("The percentage is: ");
			Serial.println(percentage);
			*/
			if(percentage > PRECISION) {
				lightState =! lightState;
				digitalWrite(outPin, lightState);
			}
		}
		else {
			// is evaluating, update the counter
			if(currentState == LOW) 
				++countLow;
			else
				++countHigh;
		}
	}
	else if(lastState == LOW && currentState == HIGH && (millis() - lastPressTime > BLOCKPRESS)) { // if the button is pressed and is not evaluating start the evaluation time
		// Serial.println("Start evaluating");
		isEvaluating = true;
		eStartTime = millis();
		resetCounter();
	}

	lastState = currentState;
	return;
}