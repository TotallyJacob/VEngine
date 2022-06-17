#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "../Logger.hpp"

#include <assert.h>
#include <iostream>
#include <map>
#include <mutex>
#include <semaphore>

namespace vengine
{

class SyncQueue
{
    public:

        SyncQueue() = default;
        SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);
        void init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);

        void publish_queue();
        void add_sync(const GLsync sync);
        void insert_published_syncs_into(std::vector<GLsync>& to_insert_into);

        auto num_queues() -> unsigned int;
        auto pre_allocated_num_queue_elements() -> unsigned int;

    private:

        void        add_sync_to_queue(const GLsync sync, const unsigned int queue_pos);
        static auto lock_free_string(std::string initial_comment, std::atomic<unsigned int>& mutex) -> std::string;

        struct SyncBuffer
        {
                std::vector<GLsync> syncs;
                unsigned int        num_added_syncs = 0;
        };


        // These can only be modified through init -> and init should only be called once.
        unsigned int m_num_queues = 0;
        unsigned int m_pre_allocated_num_queue_elements = 0;
        //
        std::atomic<unsigned int> m_last_read_queue = 0;
        std::atomic<unsigned int> m_sync_queue_pos = 0;

        std::map<unsigned int, SyncBuffer> m_sync_queue = {};
};
} // namespace vengine
