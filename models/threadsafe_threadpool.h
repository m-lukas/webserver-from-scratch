#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>

#include "threadsafe_queue.h"

class threadsafe_threadpool
{
public:
    using Task = std::function<void()>;

    explicit threadsafe_threadpool(std::size_t numThreads){
        start(numThreads);
    }

    ~threadsafe_threadpool(){
        done=true;
    }

    void Add(Task task){
        mTasks.push(task);
    }

private:
    std::vector<std::thread> mThreads;
    std::atomic_bool done;
    threadsafe_queue<Task> mTasks;

    void start(std::size_t numThreads){
        for (auto i = 0u; i < numThreads; ++i)
        {
            mThreads.emplace_back([=] {
                while(!done){
                    {
                        Task task;
                        mTasks.wait_and_pop(task);
                        task();
                    }
                    
                }
            });
        } 
    }
};
