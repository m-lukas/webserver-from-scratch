#pragma once

#include <functional>
#include <map>

#include "socket.cpp"
#include "../http/request.h"
#include "../http/response.h"
#include "../util.cpp"

class Server
{
private:
    bool m_running;
    Socket m_socket;
    std::map<std::string, std::function<int(Request, Response)>> m_routes;
    std::string m_name;
    util::CON_MODE m_conmode;

    void listenProcess();
    void listenSingleThread();
    void listenPool();
    void listenLockFreePool();
    void listenThreadSafePool();
    void listenDispatchQueue();
    void handleRequest(Socket sock);
public:
    Server();

    void Route(std::string path, std::function<int(Request, Response)> func);
    void SetName(std::string name);
    void SetConcurrencyMode(util::CON_MODE mode){ m_conmode=mode; }
    std::string GetServerName(){ return m_name; }

    void Listen(int port);
    void Stop();
};
