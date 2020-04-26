#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <map>

#include "server.h"
#include "socket.cpp"
#include "../http/request.h"
#include "../http/response.h"
#include "../http/header.h"
#include "../logger/logger.h"

Server::Server(){
    m_running = false;
    m_socket = Socket();
}

void Server::Stop(){
    m_running = false;
}

void Server::Route(std::string path, std::function<int(Request, Response)> func){
    if(path[0] == '/') path.erase(0, 1);
    m_routes[path] = func;
};

void Server::Listen(int port){

    try
    {
        m_socket.SetOpt(REUSE_ADDRESS, 1);
        m_socket.Bind(port);
        m_socket.Listen();
    }
    catch(const std::exception& e)
    {
        logger::error(e.what());
        return;
    }
    
    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept();

        Request req = Request(sock);
        Response resp = Response(sock);

        req.Read();
        
        auto err = req.Parse();
        if(err != 0) {
            logger::debug("Request Parsing Failed");
            resp.Status(500)->Error();
            continue;
        }

        handleRequest(req, resp);

        //sock.Close();
    }

    m_socket.Close();
}

void Server::handleRequest(Request req, Response resp){
    Method method = req.getHeader().getMethod();
    std::string path = req.getHeader().getPath();

    switch (method)
    {
    case GET:
        if(m_routes.count(path) == 1) m_routes[path](req, resp);
        break;
    default:
        resp.Status(501)->Error();
        break;
    }

    return;
}

//string_view or span > later

//index.html redirection
//static routes
//read resources for LU
//Look into async request handling
//create custom writer to simplify writing stuff