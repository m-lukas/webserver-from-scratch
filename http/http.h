#pragma once

using namespace std;

enum Method 
{    
    GET,
    POST,
    PUT,
    DELETE,
    CONNECT,
    HEAD,
    OPTIONS,
    PATCH,
    TRACE,
    UNKNOWN = -1,
};

Method getMethod(string method){
    if(method == "GET") return GET;
    if(method == "POST") return POST;
    if(method == "PUT") return PUT;
    if(method == "DELETE") return DELETE;
    if(method == "CONNECT") return CONNECT;
    if(method == "HEAD") return HEAD;
    if(method == "OPTIONS") return OPTIONS;
    if(method == "PATCH") return PATCH;
    if(method == "TRACE") return TRACE;

    return UNKNOWN;
}

bool isValidVersion(string version){
    if(version == "HTTP/1.1"){
        return true;
    }

    return false;
}