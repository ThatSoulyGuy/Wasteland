#pragma once

#include <functional>
#include <queue>

namespace Wasteland::Thread
{
    class MainThreadExecutor final
    {
    
    public:

        MainThreadExecutor(const MainThreadExecutor&) = delete;
        MainThreadExecutor(MainThreadExecutor&&) = delete;
        MainThreadExecutor& operator=(const MainThreadExecutor&) = delete;
        MainThreadExecutor& operator=(MainThreadExecutor&&) = delete;

        void EnqueueTask(void* holder, std::function<void()> task)
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(std::make_tuple(holder, std::move(task)));
        }

        void CancelTask(void* holder)
        {
            std::lock_guard<std::mutex> lock(mutex);

            std::queue<std::tuple<void*, std::function<void()>>> newQueue;

            while (!tasks.empty())
            {
                auto& front = tasks.front();

                if (std::get<0>(front) != holder)
                    newQueue.push(std::move(front));

                tasks.pop();
            }

            tasks.swap(newQueue);
        }

        void Execute()
        {
            std::queue<std::tuple<void*, std::function<void()>>> localQueue;

            {
                std::lock_guard<std::mutex> lock(mutex);

                localQueue.swap(tasks);
            }

            while (!localQueue.empty())
            {
                auto& fn = localQueue.front();

                if (std::get<0>(fn))
                    std::get<1>(fn)();

                localQueue.pop();
            }
        }

        static MainThreadExecutor& GetInstance()
        {
            std::call_once(initializationFlag, [&]()
            {
                instance = std::unique_ptr<MainThreadExecutor>(new MainThreadExecutor());
            });

            return *instance;
        }

    private:

        MainThreadExecutor() = default;

        std::mutex mutex;
        std::queue<std::tuple<void*, std::function<void()>>> tasks;

        static std::once_flag initializationFlag;
        static std::unique_ptr<MainThreadExecutor> instance;

    };

    std::once_flag MainThreadExecutor::initializationFlag;
    std::unique_ptr<MainThreadExecutor> MainThreadExecutor::instance;
}