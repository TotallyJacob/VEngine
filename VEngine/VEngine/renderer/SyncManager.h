#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <windows.h>

#include <array>
#include <iostream>
#include <mutex>
#include <thread>

#include "../Logger.hpp"
#include "SyncQueue.h"

namespace vengine
{


#define SYNC_MANAGER_IDENTIFIER <num_mutable_buffers>
#define SYNC_MANAGER_TEMPLATE   template <unsigned int num_mutable_buffers>

SYNC_MANAGER_TEMPLATE
class SyncManager
{

    public:

        SyncManager(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size);
        ~SyncManager();

        SyncManager(const SyncManager&) = delete;

        void register_mutable_buffer(unsigned int& buffer_id)
        {
            if (m_sync_queue.is_publisher_thread())
            {
                assert(m_num_mutable_buffers < num_mutable_buffers && "Too many registered mutable_buffers");
                buffer_id = m_num_mutable_buffers;
                m_num_mutable_buffers++;
            }
        }

        void add_sync(const GLsync sync, unsigned int buffer_id);
        void publish_added_syncs()
        {
            m_sync_queue.publish_queue();
        }
        void make_publisher_thread()
        {
            m_sync_queue.make_publisher_thread();
        }


        void stop();

        auto is_sync_signaled(const unsigned int buffer_id) -> bool
        {
            assert(buffer_id < num_mutable_buffers && "Too many registered mutable_buffers");

            if (m_sync_queue.is_publisher_thread())
            {
                bool is_signaled = m_mutable_buffer_syncs_signaled.at(buffer_id);

                if (is_signaled)
                {
                    m_mutable_buffer_syncs_signaled.at(buffer_id) = false;
                }

                return is_signaled;
            }

            return false;
        }

    private:

        void init(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size);
        void thread_sync_handler(GLenum& result);
        void thread_function(HDC hdc, HGLRC hglrc);

        std::unique_ptr<std::thread> m_thread;

        // running
        std::mutex        running_mutex{};
        std::atomic<bool> m_running = true;

        std::array<std::atomic<bool>, num_mutable_buffers> m_mutable_buffer_syncs_signaled = {true};

        // syncs
        unsigned int                 m_num_mutable_buffers = 0;
        std::vector<SyncQueue::Sync> m_syncs{};
        SyncQueue                    m_sync_queue;
};

}; // namespace vengine

#include "SyncManager.ipp"

#undef SYNC_MANAGER_IDENTIFIER
#undef SYNC_MANAGER_TEMPLATE