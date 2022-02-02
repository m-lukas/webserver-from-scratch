#pragma once

#include <thread>
#include <vector>
#include <utility>

//Class joining_threads is based on examples in the book "C++ - Concurrency in Action" (ISBN: 9781617294693)

class join_threads
{
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_):
        threads(threads_)
    {}
    ~join_threads()
    {
        for(unsigned long i=0;i<threads.size();++i)
        {
            if(threads[i].joinable())
                threads[i].join();
        }  
    }
};