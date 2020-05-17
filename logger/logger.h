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
#include <condition_variable>
#include <thread>
#include <queue>

#include "../util.cpp"
#include "../core/threadpool.h"

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
    private:
        std::condition_variable mNotifyVar;
        std::thread mlogThread;
        std::mutex mNotifyMutex;
        std::queue<std::string> mLogs;

        void _log(std::string);
        void startLogThread();
        void writeToFile(logLevel lvl, std::string logMsg);
    public:
        ofstream mOut;
        logLevel mLogLevel;

        template<typename... Args>
        void log(enum logLevel lvl, const char *msg, Args... args);

        logger(string outputPath, enum logLevel userLvl): mOut{ ofstream(outputPath, ios_base::app) }, mLogLevel{userLvl} {}
};

logger defaultLogger{"log.out", WARN};

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

void SetLevel(logLevel userLvl){
    defaultLogger.mLogLevel = userLvl;
    return;
}

template<typename... Args>
void fatal(const char *msg, Args... args){
    defaultLogger.log(FATAL, msg, args...);
}

template<typename... Args>
void error(const char *msg, Args... args){
    defaultLogger.log(ERROR, msg, args...);
}

template<typename... Args>
void warn(const char *msg, Args... args){
    defaultLogger.log(WARN, msg, args...);
}

template<typename... Args>
void info(const char *msg, Args... args){
    defaultLogger.log(INFO, msg, args...);
}

template<typename... Args>
void debug(const char *msg, Args... args){
    defaultLogger.log(DEBUG, msg, args...);
}

template<typename... Args>
void logger::log(logLevel lvl, const char *msg, Args... args){
    std::string logMsg;
    char buffer[150]; //std::array<char, 150>
    snprintf(buffer, 150, msg, args...);

    logMsg += util::GetTimeString("%Y-%m-%d %X");
    logMsg += " ";
    logMsg += getLogTag(lvl);
    logMsg += " ";
    logMsg += buffer;

    {
        std::unique_lock<std::mutex> lock{mNotifyMutex};
        mLogs.emplace(logMsg);
        printf("Size: %d\n", mLogs.size());
    }

    mNotifyVar.notify_one();
}

void logger::_log(std::string logMsg){
    printf("%s\n", logMsg.c_str()); //printf = C Function

    /*
    if(lvl <= logLevel){
        printf("%s\n", logMsg); //printf = C Function
    }

    if (lvl <= WARN){
        writeToFile(lvl, buffer);
    }
    */
}

void logger::writeToFile(logLevel lvl, std::string logMsg){
    if(mOut.is_open()){
        mOut << util::GetTimeString("%Y-%m-%d %X").c_str() << " " << getLogTag(lvl).c_str() << " " << logMsg.c_str() << "\n";
        mOut.flush();
    }
    return;
}

void logger::startLogThread(){
    mlogThread = std::thread([=] {
        while(true){
            std::string logMsg;

            {
                std::unique_lock<std::mutex> lock{mNotifyMutex};
                mNotifyVar.wait(lock, [=] { return !mLogs.empty(); });

                if(mLogs.empty())
                    break;

                logMsg = std::move(mLogs.front());
                mLogs.pop();
            }

            _log(logMsg);
        }
    });
    mlogThread.detach();
}

}