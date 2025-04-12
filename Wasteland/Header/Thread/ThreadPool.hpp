#pragma once


#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <array>

namespace Wasteland::Thread
{
    template <size_t N>
    class ThreadPool
    {

    public:

        ThreadPool()
        {
            for (size_t i = 0; i < N; ++i)
                workers[i] = std::thread(&ThreadPool::WorkerThread, this);
        }
    
        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                stop = true;
            }

            conditionVariable.notify_all();
    
            for (auto& w : workers)
            {
                if (w.joinable())
                    w.join();
            }
        }
    
        void EnqueueTask(std::function<void()> task)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                tasks.push(std::move(task));
            }

            conditionVariable.notify_one();
        }
    
    private:

        void WorkerThread()
        {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex);

                    conditionVariable.wait(lock, [this]{ return !tasks.empty() || stop; });
    
                    if (stop && tasks.empty())
                        return;
    
                    task = std::move(tasks.front());

                    tasks.pop();
                }
                
                task();
            }
        }
    
        std::array<std::thread, 3> workers;
    
        std::mutex mutex;
        std::condition_variable conditionVariable;
        std::queue<std::function<void()>> tasks;

        bool stop = false;
    };
}