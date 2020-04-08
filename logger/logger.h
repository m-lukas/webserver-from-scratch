#pragma once

#include <cstdio>
#include <stdio.h>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

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
string getConsoleTimeString();
string getOutputTimeString();
void writeToFile(logLevel lvl, char *buffer);

struct logger
{
    ofstream out;

    logger(string outputPath){
        out = ofstream(outputPath, ios_base::app);
    }
};

logger defaultLogger = logger("log.out");

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

    //CONSULTING - How to buffer?
    char buffer[150];
    sprintf(buffer, msg, args...);
    printf("%s %s %s\n", getConsoleTimeString().c_str(), getLogTag(lvl).c_str(), buffer);

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

string getConsoleTimeString(){
    char s[1000];

    time_t now = time(0);
    tm *ltm = localtime(&now);
    strftime(s, 1000, "%X", ltm);

    return s;
}

string getOutputTimeString(){
    char s[1000];

    time_t now = time(0);
    tm *ltm = localtime(&now);
    strftime(s, 1000, "%Y-%m-%d %X", ltm);
    
    return s;
}

void writeToFile(logLevel lvl, char *buffer){
    if(defaultLogger.out.is_open()){
        defaultLogger.out << getOutputTimeString() << " " << getLogTag(lvl).c_str() << " " << buffer << "\n";
        defaultLogger.out.flush();
    }
}

}