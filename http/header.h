#pragma once

#include <map>
#include <stdexcept>
#include <vector>

#include "http.h"
#include "../core/socket.hpp"
#include "../util.cpp"

class Header
{
private:
    Method h_method;
    int h_statuscode;
    std::string h_version, h_path;
    std::map<std::string, std::string> h_fields, h_parameters;
public:
    Header();
    ~Header();

    void setMethod(Method m){ h_method = m; }
    void setVersion(std::string v){ h_version = v; }
    void setPath(std::string p){ h_path = p; }
    void setStatus(int sc){ h_statuscode = sc; }
    void demarshallQuery(std::string queryStr);

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

void Header::demarshallQuery(std::string queryStr){
    std::vector<std::string> params = util::Split(queryStr, '&', 100);
    for(auto const& item: params) {
        std::string key;
        std::string val;

        std::vector<std::string> keyVal = util::Split(item, '=', 1);
        if(keyVal.size() > 1){
            key = keyVal[0];
            val = keyVal[1];
        }
        h_parameters[key] = val;
    }
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
    sock.Write(header, strlen(header));
}
