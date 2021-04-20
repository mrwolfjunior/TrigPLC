#ifndef Light_h
#define Light_h

#include <Arduino.h>

class Light
{
	private:
                uint8_t pin;
                int state;
        public:
                /**
                 * @brief Construct a new Light object
                 * 
                 * @param pin Light pin
                 */
                Light(int pin);
                /**
                 * @brief Function to be called in the setup function
                 * 
                 */
                void setup(void);
                /**
                 * @brief Set the light state
                 * 
                 * @param newState HIGH or LOW
                 */
                void setState(int newState);
                /**
                 * @brief Invert current state, if HIGH --> LOW, if LOW --> HIGH
                 * 
                 * @return int Current state
                 */
                int triggerLight(void);
                /**
                 * @brief Get current light state
                 * 
                 * @return int Current state
                 */
                int getState(void);
};

#endif
