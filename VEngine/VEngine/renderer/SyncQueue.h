#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "../Logger.hpp"

#include <assert.h>
#include <iostream>
#include <map>
#include <mutex>

namespace vengine
{

class SyncQueue
{
    public:

        struct SyncBuffer
        {
                std::vector<GLsync> syncs;
                unsigned int        num_added_syncs = 0;
        };

        SyncQueue() = default;
        SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
        {
            init(num_queues, approximate_num_elements_per_queue);
        }
        void init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
        {
            assert(num_queues > 1 && "most be atleast 2 queues to work.");

            m_last_read_queue = 0;
            m_sync_queue_pos = 0;
            m_pre_allocated_num_queue_elements = approximate_num_elements_per_queue;
            m_num_queues = num_queues;

            SyncBuffer buffer;
            buffer.syncs.resize(approximate_num_elements_per_queue);

            for (int i = 0; i < m_num_queues; i++)
            {
                m_sync_queue.emplace(i, buffer);
            }
        }

        void publish_queue()
        {

            // Maybe add mutex
            unsigned int current_queue_pos = m_sync_queue_pos;
            unsigned int new_queue_pos = (current_queue_pos + 1) % m_num_queues;

            if (new_queue_pos != m_last_read_queue)
            {
                m_sync_queue_pos = new_queue_pos;
            }
        }

        void add_sync(const GLsync sync)
        {
            add_sync_to_queue(sync, m_sync_queue_pos);
        }

        void insert_published_syncs_into(std::vector<GLsync>& to_insert_into)
        {
            unsigned int i = m_last_read_queue;
            unsigned int sync_queue_pos = m_sync_queue_pos;

            if (i == sync_queue_pos)
            {
                // VE_LOG_WARNING("SyncQueue: last_read_queue == sync_queue_pos");
                return;
            }

            bool         no_exit = true;
            unsigned int final_i = i;
            while (no_exit)
            {
                if (i == sync_queue_pos)
                {
                    no_exit = false;
                    break;
                }

                auto& queue = m_sync_queue.at(i);
                auto& buffer = queue.syncs;
                auto  syncs_to_add = queue.num_added_syncs;

                unsigned int tally = 0;
                for (auto abc : buffer)
                {
                    if (abc == 0)
                    {
                        tally++;
                    }
                }

                VE_LOG_WARNING("tally: " << tally);

                to_insert_into.insert(to_insert_into.end(), buffer.begin(), buffer.begin() + syncs_to_add);

                queue.num_added_syncs = 0;


                final_i = i;
                i = (i + 1) % m_num_queues;
            }

            m_last_read_queue = final_i;

            if (m_last_read_queue == sync_queue_pos)

            {
                VE_LOG_ERROR("BIG TIME ERROR, SyncQueue, published_synces.");
            }
        }

        auto num_queues() -> unsigned int
        {
            return m_num_queues;
        }

        auto pre_allocated_num_queue_elements() -> unsigned int
        {
            return m_pre_allocated_num_queue_elements;
        }

    private:

        void add_sync_to_queue(const GLsync sync, const unsigned int queue_pos)
        {
            assert(queue_pos < m_num_queues && "queue_pos too large.");

            auto& queue = m_sync_queue.at(queue_pos);
            auto& buffer = queue.syncs;
            auto& num_added_syncs = queue.num_added_syncs;

            if (num_added_syncs == buffer.size())
            {
                buffer.push_back(sync);
            }
            else
            {
                buffer.at(num_added_syncs) = sync;
            }

            num_added_syncs++;
        }

        unsigned int m_num_queues = 0;
        unsigned int m_pre_allocated_num_queue_elements = 0;

        std::atomic<unsigned int> m_last_read_queue = {};
        // std::atomic<bool>         m_use_overflow_queue = false;
        std::atomic<unsigned int> m_sync_queue_pos = {};

        std::map<unsigned int, SyncBuffer> m_sync_queue = {};
};
} // namespace vengine
