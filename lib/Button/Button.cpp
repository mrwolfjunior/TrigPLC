#include <Button.h>

Button::Button(int pin)
{
	this->pin = pin;
	this->initFlag = false;
}

void Button::setup(void)
{
	if (initFlag == false)
	{
		pinMode(pin, INPUT);
		initFlag = true;
	}

	lastState = digitalRead(pin);
	currentState = lastState;
}

void Button::update(void)
{
	lastState = currentState;
	currentState = digitalRead(pin);
}

boolean Button::wasPressed(void)
{
	return (currentState == LOW && lastState == HIGH);
}

boolean Button::isPressed(void)
{
    update();
	return wasPressed();
}