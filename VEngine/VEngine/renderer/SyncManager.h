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
#include "ConcSyncInserter.h"

namespace vengine
{


#define SYNC_MANAGER_IDENTIFIER <num_mutable_buffers, license_active>
#define SYNC_MANAGER_TEMPLATE   template <unsigned int num_mutable_buffers, const bool license_active>

template <unsigned int num_mutable_buffers, const bool license_active = false>
class SyncManager
{

    public:

        SyncManager();
        ~SyncManager();

        void init();

        SyncManager(const SyncManager&) = delete;

        auto register_mutable_buffer(unsigned int& buffer_id) -> std::optional<SyncInserter&>
        {
            if (m_publisher_license.thread_has_license())
            {
                assert(m_num_mutable_buffers < num_mutable_buffers && "Too many registered mutable_buffers");
                buffer_id = m_num_mutable_buffers;
                m_num_mutable_buffers++;

                return m_sync_inserters.at(buffer_id);
            }

            return {};
        }

        void make_publisher_thread()
        {
            if (!m_publisher_license.try_aquire())
            {
                VE_LOG_ERROR("Publisher cannot aquire license.");
                assert(false);
            }
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

        std::array<ConcSyncInserter, num_mutable_buffers> m_sync_inserters = {};
        ThreadLicense<license_active>                     m_publisher_license;
        unsigned int                                      m_num_mutable_buffers = 0;
};

}; // namespace vengine

#include "SyncManager.ipp"

#undef SYNC_MANAGER_IDENTIFIER
#undef SYNC_MANAGER_TEMPLATE