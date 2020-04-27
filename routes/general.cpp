#include <functional>

#include "../http/request.h"
#include "../http/response.h"

int getIndex(Request req, Response resp){
    resp.SendFile("index.html");
}

std::function<int(Request, Response)> f_getIndex = getIndex;
