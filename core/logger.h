#pragma once

#include <cstdio>
#include <stdio.h>
#include <ctime>
#include <string>

namespace logger {

enum logLevel{
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
};

std::string getLogTag(logLevel lvl);
std::string getConsoleTimeString();

std::string getLogTag(logLevel lvl){
    switch (lvl)
    {
    case FATAL:
        return "[FATAL]";
    case ERROR:
        return "[ERROR]";
    case WARN:
        return "[WARN]";
    case INFO:
        return "[INFO]";
    case DEBUG:
        return "[DEBUG]";
    
    default:
        return "[LOG]";
    }
}

struct logger
{
    
};

template<typename... Args>
void error(const char *msg, Args... args){
    log(ERROR, msg, args...);
}

template<typename... Args>
void log(logLevel lvl, const char *msg, Args... args){
    char buffer[150];
    sprintf(buffer, msg, args...);
    printf("%s %s %s\n", getConsoleTimeString().c_str(), getLogTag(lvl).c_str(), buffer);
}

std::string getConsoleTimeString(){
    std::string timeStr;

    time_t now = time(0);
    tm *ltm = localtime(&now);

    int hour = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec;
    
    timeStr = std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second);
    return timeStr;
}

}


