
#include "../http/request.h"
#include "../http/response.h"
#include "../http/http.h"

int getIndex(Request req, Response resp){
    resp.SendFile("index.html");
}

int getHello(Request req, Response resp){
    resp.Send("Hello World!");
}