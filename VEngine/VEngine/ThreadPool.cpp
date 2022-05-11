#include "ThreadPool.h"

using namespace vengine;

ThreadPool::ThreadPool(const unsigned int num_threads)
{
    // std::latch latch(num_threads);

    std::vector<std::thread> threads;

    std::condition_variable cv;

    std::mutex        mtx;
    std::shared_mutex smtx, wait_mutex;
    bool              condition = false;
    bool              second_phase = false;

    for (int i = 0; i < num_threads; i++)
    {
        threads.push_back(std::thread(
            [&]()
            {
                int  cpy = 0;
                bool con = false;
                {
                    std::unique_lock<std::mutex> lk(mtx);
                    int                          cpy = i;
                    bool                         con = condition;
                    VE_LOG_DEBUG("Waiting " << cpy);
                }

                if (cpy == num_threads)
                {
                    VE_LOG_DEBUG("second phase signaled");
                    std::unique_lock<std::shared_mutex> smtx(wait_mutex);
                    second_phase = true;
                }

                std::unique_lock<std::mutex> lk(mtx);
                while (!con)
                {
                    cv.wait(lk);
                    std::shared_lock<std::shared_mutex> s(smtx);
                    con = condition;
                }

                VE_LOG_DEBUG("Going " << cpy);
            }));
    }
    bool test = false;
    while (!test)
    {
        std::shared_lock<std::shared_mutex> l(wait_mutex);
        test = second_phase;
        VE_LOG_DEBUG("second phase loop.");
    }

    {
        std::scoped_lock<std::shared_mutex, std::mutex> sl(smtx, mtx);
        condition = true;
        cv.notify_all();
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}