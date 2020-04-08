#pragma once

#include <cstdio>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <array>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

namespace logger {

enum logLevel{
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
};

string getLogTag(logLevel lvl);
string getTimeString(string format);
void writeToFile(logLevel lvl, char *buffer);

void SetLevel(logLevel userLvl);

struct logger
{
    ofstream out;
    logLevel logLevel;

    logger(string outputPath, enum logLevel userLvl): out{ ofstream(outputPath, ios_base::app) }, logLevel{userLvl}  {}
};

logger defaultLogger = logger("log.out", WARN);

string getLogTag(logLevel lvl){
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

template<typename... Args>
void fatal(const char *msg, Args... args){
    log(FATAL, msg, args...);
}

template<typename... Args>
void error(const char *msg, Args... args){
    log(ERROR, msg, args...);
}

template<typename... Args>
void warn(const char *msg, Args... args){
    log(WARN, msg, args...);
}

template<typename... Args>
void info(const char *msg, Args... args){
    log(INFO, msg, args...);
}

template<typename... Args>
void debug(const char *msg, Args... args){
    log(DEBUG, msg, args...);
}

template<typename... Args>
void log(logLevel lvl, const char *msg, Args... args){

    char buffer[150]; //std::array<char, 150>
    snprintf(buffer, 150, msg, args...);

    if(lvl <= defaultLogger.logLevel){
        printf("%s %s %s\n", getTimeString("%X").c_str(), getLogTag(lvl).c_str(), buffer); //printf = C Function
    }

    switch (lvl)
    {
        case FATAL:
            writeToFile(lvl, buffer);
            break;
        case ERROR:
            writeToFile(lvl, buffer);
            break;
        case WARN:
            writeToFile(lvl, buffer);
            break; 
        default:
            break;
    }
}

string getTimeString(string format){
    // %Y year
    // %m year
    // %d year
    // %X time hh:mm:ss

    std::array<char, 1000> s;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), format.c_str());
    return ss.str();
}

void writeToFile(logLevel lvl, char *buffer){
    if(defaultLogger.out.is_open()){
        defaultLogger.out << getTimeString("%Y-%m-%d %X").c_str() << " " << getLogTag(lvl).c_str() << " " << buffer << "\n";
        defaultLogger.out.flush();
    }
    return;
}

void SetLevel(logLevel userLvl){
    defaultLogger.logLevel = userLvl;
    return;
}

}