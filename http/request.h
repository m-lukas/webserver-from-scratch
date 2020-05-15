#pragma once

#include "header.h"
#include "../core/socket.hpp"
#include "../util.cpp"
#include "http.h"

class Request
{
private:
    Header r_header;
    std::string r_body;
    Socket r_socket;
    char r_raw[30000];
public:
    Request(Socket sock) : r_socket(sock) {
        r_header = Header();
    };

    Header getHeader() { return r_header; }
    std::string getBody() { return r_body; }

    long Read();
    int Parse();
};

long Request::Read(){
    memset(r_raw, 0, 30000);
    long valread = r_socket.Read(r_raw);
    //printf("%s\n", r_raw);
    return valread;
}

int Request::Parse(){
    const char *msg = const_cast<const char*>(r_raw);

    std::stringstream ss(msg);
    std::string line;
    int contentLength;

    r_header.Clear();
    
    if(getline(ss, line, '\n')){
        contentLength += line.size()+1;

        std::vector<std::string> v = util::Split(line, ' ', 2);
        if(v.size() != 3) return -1; //invalid header structure

        std::string methodStr = v.at(0);
        std::string pathStr = v.at(1);
        std::string versionStr = v.at(2);

        if(!pathStr.empty()){
            std::vector<std::string> pathParts = util::Split(pathStr, '?', 1);
            if(pathParts.size() > 1){
                r_header.demarshallQuery(pathParts[1]); //pathParts[1] = query string
                pathStr = pathParts[0];
            } 

            if(pathStr[0] == '/') pathStr.erase(0, 1);
        }

        if(!versionStr.empty() && versionStr[versionStr.size() - 1] == '\r') versionStr.erase(versionStr.size() - 1);
        if(!isValidVersion(versionStr)) return -2; //invalid http version

        Method method = getMethod(methodStr);
        if(method == UNKNOWN) return -3; //unknown http method

        r_header.setMethod(method);
        r_header.setPath(pathStr);
        r_header.setVersion(versionStr);
    }

    while(getline(ss, line, '\n')){
        contentLength += line.size()+1;

        if(line == "\r"){
            break; //end of header
        }

        std::vector<std::string> v = util::Split(line, ':', 1);
        if(v.size() != 2) return -4;

        std::string field = v.at(0);
        std::string value = v.at(1);

        if(!value.empty() && value[value.size() - 1] == '\r') value.erase(value.size() - 1);

        r_header.Add(field, value);
    }

    if(contentLength < strlen(msg)){
        r_body = ss.str().substr(contentLength, std::string::npos);
    }
    return 0;
}