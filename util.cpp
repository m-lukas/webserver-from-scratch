#pragma once

#include <sstream>
#include<vector>

/*
std::vector<std::string> SplitOnce(std::string s, char delimiter){
    std::vector<std::string> v = SplitFunc(s, delimiter, 1);
    return v;
}

std::vector<std::string> Split(std::string s, char delimiter, int cases){
    std::vector<std::string> v = SplitFunc(s, delimiter, cases);
    return v;
}
*/

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

