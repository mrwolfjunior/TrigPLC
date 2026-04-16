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
                 * @brief Read the pin and update state (call once per loop)
                 * 
                 */
                void update(void);
                /**
                 * @brief Check if button was pressed since last update
                 * 
                 * @return boolean
                 */
                boolean wasPressed(void);
                /**
                 * @brief Deprecated: use update() + wasPressed()
                 * 
                 * @return boolean
                 */
                boolean isPressed(void);
};

#endif
