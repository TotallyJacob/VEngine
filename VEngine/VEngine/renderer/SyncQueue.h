#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "../Logger.hpp"

#include <assert.h>
#include <iostream>
#include <map>
#include <mutex>
#include <semaphore>


// You can define this protection to be sure only two threads are messing with the data.
#define SYNC_QUEUE_MUTEX_PROTECTION
#ifdef SYNC_QUEUE_MUTEX_PROTECTION
#define SYNC_QUEUE_MUTEX_DO(x) x
#else
#define SYNC_QUEUE_MUTEX_DO(x)
#endif

namespace vengine
{

class SyncQueue
{
    public:

        SyncQueue() = default;
        SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);
        void init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);

        void make_publisher_thread();
        void make_reader_thread();

        void publish_queue();
        void add_sync(const GLsync sync);
        void insert_published_syncs_into(std::vector<GLsync>& to_insert_into);

        auto num_queues() -> unsigned int;
        auto pre_allocated_num_queue_elements() -> unsigned int;

    private:

        auto publisher_try_lock() -> bool;
        auto reader_try_lock() -> bool;

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

        // Queue data
        std::atomic<unsigned int>          m_last_read_queue = 0;
        std::atomic<unsigned int>          m_sync_queue_pos = 0;
        std::map<unsigned int, SyncBuffer> m_sync_queue = {};

        // Mutex protection data
        SYNC_QUEUE_MUTEX_DO(std::atomic<bool>    m_publisher_thread_exists = false; //
                            std::atomic<bool>    m_reader_thread_exists = false;    //
                            std::recursive_mutex m_publisher_mutex = {};            //
                            std::recursive_mutex m_reader_mutex = {};               //
        )
};
} // namespace vengine
