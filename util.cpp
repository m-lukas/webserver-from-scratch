#pragma once

#include <sstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <array>
#include <sys/types.h>
#include <sys/wait.h>

namespace util{

enum CON_MODE {
    CON_MODE_POOL,
    CON_MODE_PROCESS,
    CON_MODE_SINGLE_THREAD
};

std::vector<std::string> Split(std::string s, char delimiter, int cases){
    std::vector<std::string> v;

    if(s.size() == 0){
        return v;
    }

    std::stringstream ss(s);
    std::string line;
    int i = 0;

    while(getline(ss, line, delimiter)){
        if(i > cases){
            std::string lastElem = v.at(v.size()-1);
            v.at(v.size()-1) = (lastElem += delimiter + line);
        }else{
            v.push_back(line);
        }

        i++;
    }

    return v;
}

std::string GetTimeString(std::string format){
    // %Y year
    // %m month
    // %d day
    // %X time hh:mm:ss
    // %Z timezone (if available)
    // %a weekday

    std::array<char, 2000> s;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), format.c_str());
    return ss.str();
}

void RemoveZombies(int sig){
    while (true)
    {
        pid_t pid = 0;
        int status;

        try
        {
            pid = waitpid(-1, &status, WNOHANG);
        }
        catch(const std::exception& e)
        {
            return;
        }

        if(pid == 0){
            return; //no more zombies
        }
    }
}
   
}

