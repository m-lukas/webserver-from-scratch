#pragma once

#include <functional>
#include <map>

#include "socket.hpp"
#include "../http/request.h"
#include "../http/response.h"

class Server
{
private:
    bool m_running;
    Socket m_socket;
    std::map<std::string, std::function<int(Request, Response)>> m_routes;
    std::string m_name;

    void handleRequest(Socket sock);
public:
    Server();

    void Route(std::string path, std::function<int(Request, Response)> func);
    void SetName(std::string name);
    std::string GetServerName(){ return m_name; }

    void Listen(int port);
    void Stop();
};
