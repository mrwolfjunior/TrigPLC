#include <Logger.h>

Logger::Logger(int level) {
    logging_level = level;
}

void Logger::setup(int boundrate) {
    Serial.begin(boundrate);
    return;
}

void Logger::info(String log) {
    if(logging_level > NOLOG) {
        Serial.println("[ INFO ] " + log);
        return;
    }
}

void Logger::debug(String log) {
    if(logging_level > INFO) {
        Serial.println("[ DEBUG ] " + log);
        return;
    }
    return;
}

void Logger::silly(String log) {
    if(logging_level > DEBUG) {
        Serial.println("[ SILLY ] " + log);
        return;
    }
    return;
}