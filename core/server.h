#pragma once

#include <functional>
#include <map>

#include "socket.h"
#include "../http/request.h"
#include "../http/response.h"

class Server
{
private:
    bool m_running;
    Socket m_socket;
    std::map<std::string, std::function<int(Request, Response)>> m_routes;
public:
    Server();

    void Route(std::string path, std::function<int(Request, Response)> func);

    void Listen(int port);
    void Stop();

    void handleRequest(Socket sock);
};
