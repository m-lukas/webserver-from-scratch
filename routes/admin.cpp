#include <functional>

#include "../http/request.h"
#include "../http/response.h"

int getHello(Request req, Response resp){
    resp.Send("Hello World!");
}

std::function<int(Request, Response)> f_getHello = getHello;
