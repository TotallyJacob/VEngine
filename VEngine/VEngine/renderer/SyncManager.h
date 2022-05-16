#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <windows.h>

#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>

#include "../Logger.hpp"

namespace vengine
{


class SyncManager
{

    public:

        SyncManager();
        ~SyncManager();

        SyncManager(const SyncManager&) = delete;

        void add_sync(const GLsync sync);

        void stop();

    private:

        void init();

        std::unique_ptr<std::thread> m_thread;
        std::mutex                   running_mutex{};
        std::atomic<bool>            m_running = true;
        std::mutex                   m_sync_mutex{};
        std::vector<GLsync>          m_syncs{};
};

}; // namespace vengine
