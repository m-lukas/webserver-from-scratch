#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>

#include "lock_free_queue.h"

class lockfree_threadpool
{
public:
    using Task = std::function<void()>;

    explicit lockfree_threadpool(std::size_t numThreads){
        start(numThreads);
    }

    ~lockfree_threadpool(){
        done=true;
    }

    void Add(Task task){
        mTasks.push(task);
    }

private:
    std::vector<std::thread> mThreads;
    std::atomic_bool done;
    lock_free_queue<Task> mTasks;

    void start(std::size_t numThreads){
        for (auto i = 0u; i < numThreads; ++i)
        {
            mThreads.emplace_back([=] {
                while(!done){
                    auto tid = std::this_thread::get_id();
                    {
                        std::unique_ptr<Task> res = mTasks.pop();
                        if(res != std::unique_ptr<Task>())
                        {
                            Task task = *res.get();
                            task();
                        }
                        // else
                        // {
                        //     std::this_thread::sleep_for(std::chrono::milliseconds(20));
                        // }
                    }
                    
                }
            });
        } 
    }
};
