#pragma once

#include <dispatch/dispatch.h>
#include <functional>

class dispatch_queue
{
private:
    dispatch_queue_t queue = dispatch_queue_create("RequestHandler", NULL);
public:
    using Task = std::function<void()>;

    void add(Task task){
        dispatch_async(queue, ^{ task(); });
    }
};