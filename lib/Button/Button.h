#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
	private:
                int inPin;
                int outPin;
                unsigned long debounceTime;
                unsigned long lastDebounceTime; // the last time the output pin was toggled
                int currentState;
                int previousSteadyState;  // the previous steady state from the input pin, used to detect pressed and released event
		int lastSteadyState;      // the last steady state from the input pin
		int lastFlickerableState; // the last flickerable state from the input pin
                int lightState;
        public:
                Button(int in, int out);
                int getInPin(void);
                int getOutPin(void);
                int getLightState(void);
                bool isReleased(void);
                bool isPressed(void);
                void triggerLight(void);
                void setup(void);
                void setup(int time);
                int loop(void);
};

#endif
