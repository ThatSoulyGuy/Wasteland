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

        void EnqueueTask(std::function<void()> task)
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(std::move(task));
        }

        void Execute()
        {
            std::queue<std::function<void()>> localQueue;
            {
                std::lock_guard<std::mutex> lock(mutex);
                localQueue.swap(tasks);
            }

            while (!localQueue.empty())
            {
                auto& fn = localQueue.front();

                fn();

                localQueue.pop();
            }
        }

        static MainThreadExecutor& GetInstance()
        {
            std::call_once(initalizationFlag, [&]()
            {
                instance = std::unique_ptr<MainThreadExecutor>(new MainThreadExecutor());
            });

            return *instance;
        }

    private:

        MainThreadExecutor() = default;

        std::mutex mutex;
        std::queue<std::function<void()>> tasks;

        static std::once_flag initalizationFlag;
        static std::unique_ptr<MainThreadExecutor> instance;

    };

    std::once_flag MainThreadExecutor::initalizationFlag;
    std::unique_ptr<MainThreadExecutor> MainThreadExecutor::instance;
}