#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <map>
#include <chrono>
#include <stdio.h> 
#include <sys/types.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#include "server.h"
#include "socket.hpp"
#include "threadpool.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../http/header.h"
#include "../logger/logger.h"
#include "../util.cpp"

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

void Server::SetName(std::string name){
    m_name = name;
}

void Server::Listen(int port){
    try
    {
        m_socket.SetOpt(REUSE_ADDRESS, 1);
        m_socket.SetTimeout(10);
        m_socket.Bind(port);
        m_socket.Listen();
    }
    catch(const std::exception& e)
    {
        logger::error(e.what());
        return;
    }

    //ignore broken pipes
    signal(SIGPIPE, SIG_IGN);

    switch (m_conmode) {
    case util::CON_MODE_PROCESS:
        listenProcess();
        break;
    case util::CON_MODE_POOL:
        listenPool();
        break;
    case util::CON_MODE_SINGLE_THREAD:
        listenSingleThread();
        break;
    default:
        logger::error("unhandeled concurrency mode: %d", m_conmode);
        break;
    }
}

void Server::listenProcess(){

    signal(SIGCHLD, util::RemoveZombies);
    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept();
        if(sock.InvalidDescriptor()) continue;

        auto pid = fork();

        if(pid == 0){ //child
            m_socket.Close();
            handleRequest(sock);
            exit(0);
        }else{
            sock.Close(); //close parent copy
        }
    }

    m_socket.Close();
}

void Server::listenPool(){

    m_running = true;
    ThreadPool workers{5};

    while(m_running){
        Socket sock = m_socket.Accept(); //variable and pointer are only valid for one round of the loop - !!! POINTER MIGHT BE THE SAME IN THE NEXT ITERATION
        if(sock.InvalidDescriptor()) continue;

        workers.Add([=] () mutable -> void {
            this->handleRequest(sock);
        });
    }

    m_socket.Close();
}

void Server::listenSingleThread(){

    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept(); //variable and pointer are only valid for one round of the loop - !!! POINTER MIGHT BE THE SAME IN THE NEXT ITERATION
        if(sock.InvalidDescriptor()) continue;

        std::thread ct([=] () mutable -> void {
            this->handleRequest(sock);
        });
        ct.detach();
    }

    m_socket.Close();
}

void Server::handleRequest(Socket acceptSock){
    auto startTime = std::chrono::high_resolution_clock::now();

    Request req = Request(acceptSock);
    Response resp = Response(acceptSock, GetServerName());

    auto valread = req.Read();
    if(valread < 0){
        logger::debug("Couldn't read request - Error: %s", strerror(errno));
        acceptSock.Close();
        return;
    }
    
    auto err = req.Parse();
    if(err != 0) {
        logger::debug("Request Parsing Failed - Error: %d", err);
        resp.Status(500)->Error();
        acceptSock.Close();
        return;
    }

    Method method = req.getHeader().getMethod();
    std::string path = req.getHeader().getPath();
    auto iter = m_routes.find(path);

    auto tid = std::this_thread::get_id();

    std::cout << "Handling Request: /" << path << " on thread " << tid << std::endl;

    switch (method)
    {
    case GET:
        if(iter != m_routes.end()){
            iter->second(req,resp);
        }else{
            resp.SendFile(path);
        }
        break;
    default:
        resp.Status(501)->Error();
        break;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    logger::debug("Elapsed Time: %fs", elapsed.count());

    sleep(5);

    acceptSock.Close();

    return;
}

// ServerName / Date in header
// Configuration - via Main or Config (optional config file path)
// Different Listen functions (Fork, Threadpool, Single Thread)
// ThreadPool as part of Server Class -> stopping both
// Analyze/Understand ThreadPool functioning
// Gracefull Shutdown
// How does Cors work? - MiddleWare integration
// Cleaning up the Code - Something to refactor?
// Redirect - How I can refactor routes?