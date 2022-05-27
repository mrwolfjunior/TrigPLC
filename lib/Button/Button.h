#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
	private:
                uint8_t pin;
                int currentState;
                int lastState;
                bool initFlag;
        public:
                /**
                 * @brief Construct a new Button object
                 * 
                 * @param pin Button pin
                 */
                Button(int pin);
                /**
                 * @brief Function to be called in the setup function
                 * 
                 */
                void setup(void);
                /**
                 * @brief Function that check if button is pressed
                 * 
                 * @return boolean
                 */
                boolean isPressed(void);
};

#endif
