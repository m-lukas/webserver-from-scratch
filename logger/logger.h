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
#include <vector>

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

void SetLevel(logLevel userLvl);

struct logMessage{
    logLevel msgLvl;
    std::string msg;

    logMessage(){};
    explicit logMessage(logLevel lvl, std::string s) : msgLvl{lvl}, msg{s} {};
};

struct logger
{
    private:
        std::condition_variable mNotifyVar;
        vector<std::thread> mlogThread;
        std::mutex mNotifyMutex;
        std::queue<logMessage> mLogs;
        bool mStopping;

        void _log(logMessage);
        void writeToFile(std::string);

        void startLogThread();
    public:
        ofstream mOut;
        logLevel mLogLevel;

        template<typename... Args>
        void log(enum logLevel lvl, const char *msg, Args... args);

        logger(string outputPath, enum logLevel userLvl): mOut{ ofstream(outputPath, ios_base::app) }, mLogLevel{userLvl} {
            startLogThread();
        }
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
    char buffer[150]; //std::array<char, 150>
    snprintf(buffer, 150, msg, args...);
    std::string bufferStr(buffer);

    std::string msgStr = util::GetTimeString("%Y-%m-%d %X") + " " + getLogTag(lvl) + " " + bufferStr;
    logMessage logMsg{lvl, msgStr};

    std::cout << "Log function running on process: " << ::getpid() << std::endl;

    {
        std::unique_lock<std::mutex> lock{mNotifyMutex};
        mLogs.emplace(logMsg);
        printf("Size: %d\n", mLogs.size());
    }

    mNotifyVar.notify_one();
}

void logger::_log(logMessage logMsg){
    if(logMsg.msgLvl <= mLogLevel){
        printf("%s\n", logMsg.msg.c_str()); //printf = C Function
    }

    if(logMsg.msgLvl <= WARN){
        writeToFile(logMsg.msg);
    }
}

void logger::writeToFile(std::string logLine){
    if(mOut.is_open()){
        mOut << logLine.c_str() << "\n";
        mOut.flush();
    }
    return;
}

void logger::startLogThread(){
    mlogThread.emplace_back([=] {
        printf("Thread is starting\n");
        while(true){
            logMessage logMsg;

            {
                std::unique_lock<std::mutex> lock{mNotifyMutex};
                mNotifyVar.wait(lock, [=] { return mStopping || !mLogs.empty(); });

                if(mStopping && mLogs.empty()){
                    printf("Thread is stopping\n");
                    break;
                }

                printf("Thread is doing something\n");
                logMsg = std::move(mLogs.front());
                mLogs.pop();
            }

            _log(logMsg);
        }
    });
}

}