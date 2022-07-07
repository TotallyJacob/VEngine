#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <windows.h>

#include <array>
#include <assert.h>
#include <iostream>
#include <mutex>
#include <thread>

#include "../Logger.hpp"
#include "../ThreadLicense.h"
#include "ConcSyncBuffer.h"

namespace vengine
{


#define SYNC_MANAGER_IDENTIFIER <num_mutable_buffers, num_syncs, license_active>
#define SYNC_MANAGER_TEMPLATE   template <const unsigned int num_mutable_buffers, const unsigned int num_syncs, const bool license_active>

SYNC_MANAGER_TEMPLATE
class SyncManager
{

    public:

        SyncManager();
        ~SyncManager();

        SyncManager(const SyncManager&) = delete;
        SyncManager(SyncManager&&) = delete;

        void init();

        auto register_mutable_buffer(unsigned int& buffer_id)
        {
            buffer_id = m_num_mutable_buffers;
            m_num_mutable_buffers++;
        }

        void add_sync(unsigned int buffer_id);


        void stop();


    private:

        void init(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size);
        void thread_sync_handler(GLenum& result);
        void thread_function(HDC hdc, HGLRC hglrc);

        std::unique_ptr<std::thread> m_thread;

        // running
        std::mutex        running_mutex{};
        std::atomic<bool> m_running = true;

        ConcSyncInserter<num_syncs, license_active> m_sync_inserter = {};
        ConcSyncBuffer<num_syncs, license_active>   m_syncbuf = {};
        unsigned int                                m_num_mutable_buffers = 0;
};

}; // namespace vengine

#include "SyncManager.ipp"

#undef SYNC_MANAGER_IDENTIFIER
#undef SYNC_MANAGER_TEMPLATE