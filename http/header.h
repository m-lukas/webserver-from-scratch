#pragma once

#include <map>
#include <stdexcept>

#include "http.h"
#include "../core/socket.h"

class Header
{
private:
    Method h_method;
    int h_statuscode;
    std::string h_version, h_path;
    std::map<std::string, std::string> h_fields;
public:
    Header();
    ~Header();

    void setMethod(Method m){ h_method = m; }
    void setVersion(std::string v){ h_version = v; }
    void setPath(std::string p){ h_path = p; }
    void setStatus(int sc){ h_statuscode = sc; }

    Method getMethod(){ return h_method; }
    std::string getVersion(){ return h_version; }
    std::string getPath(){ return h_path; }
    int getStatus(){ return h_statuscode; }

    void Add(std::string field, std::string value);
    std::string Get(std::string field);
    void Remove(std::string field);
    void Clear();

    std::string Stringify();
    void Write(Socket sock);
};

Header::Header(){ 
    Clear();
    h_version = "HTTP/1.1";
}

Header::~Header()
{
}

void Header::Add(std::string field, std::string value){
    h_fields[field] = value;
}

std::string Header::Get(std::string field){
    return h_fields[field];
}

void Header::Remove(std::string field){
    h_fields.erase(field);
}

void Header::Clear(){
    h_fields.clear();
}

std::string Header::Stringify(){
    std::string headerStr;

    headerStr += (h_version + " " + std::to_string(h_statuscode) + " " + StatusCodes[h_statuscode] + "\n");
    for(auto const& x : h_fields){
        headerStr += (x.first + ": " + x.second + "\n");
    }
    headerStr += "\n";

    return headerStr;
}

void Header::Write(Socket sock){
    std::string headerStr = Stringify();

    char header[1024];
    strcpy(header, headerStr.c_str());
    sock.Write(header);
}

Header parseHeader(char* msg){
    std::stringstream ss(msg);
    std::string line;

    Header h = Header();
    h.Clear();
    
    if(getline(ss, line, '\n')){
        std::vector<std::string> v = Split(line, ' ', 2);
        if(v.size() != 3) throw std::runtime_error("invalid header");

        std::string methodStr = v.at(0);
        std::string pathStr = v.at(1);
        std::string versionStr = v.at(2);

        if(!pathStr.empty() && pathStr[0] == '/') pathStr.erase(0, 1);
        if(!versionStr.empty() && versionStr[versionStr.size() - 1] == '\r') versionStr.erase(versionStr.size() - 1);

        if(!isValidVersion(versionStr)) throw std::runtime_error("invalid http version");

        Method method = getMethod(methodStr);
        if(method == UNKNOWN) throw std::runtime_error("invalid http method");

        h.setMethod(method);
        h.setPath(pathStr);
        h.setVersion(versionStr);
    }

    while(getline(ss, line, '\n')){
        if(line == "\r"){
            break; //end of header
        }

        std::vector<std::string> v = Split(line, ':', 1);
        if(v.size() != 2) throw std::runtime_error("invalid header");

        std::string field = v.at(0);
        std::string value = v.at(1);

        if(!value.empty() && value[value.size() - 1] == '\r') value.erase(value.size() - 1);

        h.Add(field, value);
    }

    return h;
}


