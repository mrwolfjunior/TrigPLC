#include <Button.h>
#define ETIME 25
#define BLOCKPRESS 500 // 0.5 sec of block
#define PRECISION 0.40

void Button::resetCounter(void) {
	countLow = 0;
	countHigh = 0;
	return;
}

Button::Button(int pin) {
	this->pin = pin;
	lastPressTime = millis();
	eStartTime = lastPressTime;
	resetCounter();
}

void Button::setup(void) {
	// init hardware
    pinMode(pin, INPUT);

    // init private variables
    lastState = digitalRead(pin);
    currentState = lastState;
    return;
}

float calcPercentage(int high, int low) {
	return (float)high / (high + low);
}

boolean Button::isPressed(void) {
	currentState = digitalRead(pin);
	
	if(isEvaluating) { // is evaluating
		
		if(millis() - eStartTime > ETIME) { // finished evaluation time	
			isEvaluating = false;
			// update last press
			lastPressTime = millis();

			//calculate percentage
			percentage = calcPercentage(countHigh, countLow);

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
				return true;
			}
		}
		else {
			// is evaluating, update the counter
			if(currentState == LOW) 
				++countLow;
			else
				++countHigh;

			// Check for time saving on false contact
			if(millis() - eStartTime > (float)ETIME/3 && midEvaluation == false) {
				percentage = calcPercentage(countHigh, countLow);
				midEvaluation = true;
				if(percentage < PRECISION)
					isEvaluating = false;
			}
		}
	}
	else if(lastState == LOW && currentState == HIGH && (millis() - lastPressTime > BLOCKPRESS)) { // if the button is pressed and is not evaluating start the evaluation time
		// Serial.println("Start evaluating");
		isEvaluating = true;
		midEvaluation = false;
		eStartTime = millis();
		resetCounter();
	}

	lastState = currentState;
	return false;
}