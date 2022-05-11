#pragma once

#include <iostream>
#include <latch>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "Logger.hpp"

namespace vengine
{

class ThreadPool
{
    public:

        ThreadPool(const unsigned int num_threads);

    private:
};

}; // namespace vengine
