#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <future>
#include <atomic>
#include "threadsafe_queue.h"
#include "work_stealing_queue.h"
#include "join_threads.h"

//Class workstealing_threadpool is based on examples in the book "C++ - Concurrency in Action" (ISBN: 9781617294693)

template<typename T>
class workstealing_threadpool
{
    typedef std::function<void()> task_type;
    std::atomic_bool done;
    threadsafe_queue<task_type> pool_work_queue;
    std::vector<std::unique_ptr<work_stealing_queue>> queues;
    std::vector<std::thread> threads;
    join_threads joiner;
    static thread_local work_stealing_queue* local_work_queue;
    static thread_local unsigned my_index;
    void worker_thread(unsigned my_index_)
    {
        my_index=my_index_;
        local_work_queue=queues[my_index].get();
        while(!done)
        {
            run_pending_task();
        }
    }
    bool pop_task_from_local_queue(task_type& task)
    {
        return local_work_queue && local_work_queue->try_pop(task);
    }
    bool pop_task_from_pool_queue(task_type& task)
    {
        return pool_work_queue.try_pop(task);
    }
    bool pop_task_from_other_thread_queue(task_type& task)
    {
        for(unsigned i=0;i<queues.size();++i)
        {
            unsigned const index=(my_index+i+1)%queues.size();
            if(queues[index]->try_steal(task))
            {
                return true;
            }
        }
        return false;
    }
public:
    workstealing_threadpool():
        done(false), joiner(threads)
    {
        unsigned const thread_count=std::thread::hardware_concurrency();
        try
        {
            for(unsigned i=0;i<thread_count;++i)
            {
                queues.push_back(std::unique_ptr<work_stealing_queue>(
                    new work_stealing_queue
                ));
            }
            for(unsigned i=0;i<thread_count;++i)
            {
                threads.push_back(
                    std::thread(&workstealing_threadpool::worker_thread,this,i)
                );
            }
        }
        catch(...)
        {
            done=true;
            throw;
        }
    }
    ~workstealing_threadpool()
    {
        done=true;
    }
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(
        FunctionType f
    )
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(f);
        std::future<result_type> res(task.get_future());
        if(local_work_queue)
        {
            local_work_queue->push(std::move(task));
        }
        else
        {
            pool_work_queue.push(std::move(task));
        }
        return res;
    }
    void run_pending_task()
    {
        task_type task;
        if(pop_task_from_local_queue(task) ||
           pop_task_from_pool_queue(task) ||
           pop_task_from_other_thread_queue(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
};