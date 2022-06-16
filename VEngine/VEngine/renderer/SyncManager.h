#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <windows.h>

#include <iostream>
#include <map>
#include <mutex>
#include <semaphore>
#include <thread>

#include "../Logger.hpp"
#include "SyncQueue.h"

namespace vengine
{


/*
 *
 * HUGE PERF PROBLEM WITH LOCKING WITH MAIN THREAD DUE TO add_sync();
 *
 */
class SyncManager
{

    public:

        SyncManager(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size);
        ~SyncManager();

        SyncManager(const SyncManager&) = delete;

        void add_sync(const GLsync sync);
        void publish_added_syncs()
        {
            m_sync_queue.publish_queue();
        }


        void stop();

    private:

        void init(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size);
        void thread_sync_handler(GLenum& result);
        void thread_function(HDC hdc, HGLRC hglrc);

        std::unique_ptr<std::thread> m_thread;

        // running
        std::mutex        running_mutex{};
        std::atomic<bool> m_running = true;

        // syncs
        std::vector<GLsync> m_syncs{};
        SyncQueue           m_sync_queue;
};

}; // namespace vengine
