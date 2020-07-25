#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

#define NOLOG 0
#define INFO 1
#define DEBUG 2
#define SILLY 3

class Logger
{
	private:
        int logging_level;
    public:
        Logger(int level);
        void setup(int boundrate);
        void info(String log);
        void debug(String log);
        void silly(String log);
};

#endif
