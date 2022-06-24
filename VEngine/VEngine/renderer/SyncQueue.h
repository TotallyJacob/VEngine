#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "../Logger.hpp"
#include "../ThreadLicense.h"

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

#ifdef SYNC_QUEUE_MUTEX_PROTECTION
#define SYNC_QUEUE_HAS_MUTEX_PROTECTION true
#else
#define SYNC_QUEUE_HAS_MUTEX_PROTECTION false
#endif

namespace vengine
{

class SyncQueue
{
    public:

        struct Sync
        {
                GLsync       sync = 0;
                unsigned int buffer_id = 0;
        };

        SyncQueue() = default;
        SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);
        void init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue);

        void make_publisher_thread();
        void make_reader_thread();

        void publish_queue();
        void add_sync(const Sync sync);
        void insert_published_syncs_into(std::vector<Sync>& to_insert_into);

        auto num_queues() -> unsigned int;
        auto pre_allocated_num_queue_elements() -> unsigned int;

        auto is_publisher_thread() -> bool;
        auto is_reader_thread() -> bool;

    private:

        void        add_sync_to_queue(const Sync sync, const unsigned int queue_pos);
        static auto lock_free_string(std::string initial_comment, std::atomic<unsigned int>& mutex) -> std::string;

        struct SyncBuffer
        {
                std::vector<Sync> syncs;
                unsigned int      num_added_syncs = 0;
        };


        // These can only be modified through init -> and init should only be called once.
        unsigned int m_num_queues = 0;
        unsigned int m_pre_allocated_num_queue_elements = 0;
        //

        // Queue data
        std::atomic<unsigned int>          m_last_read_queue = 0;
        std::atomic<unsigned int>          m_sync_queue_pos = 0;
        std::map<unsigned int, SyncBuffer> m_sync_queue = {};

        ThreadLicense<SYNC_QUEUE_HAS_MUTEX_PROTECTION>
            m_publisher_license; // Better idea is to make_reader_thread give an object that can refertence the methods or something.s
        ThreadLicense<SYNC_QUEUE_HAS_MUTEX_PROTECTION> m_reader_license;

        // Mutex protection data
        /* SYNC_QUEUE_MUTEX_DO(std::atomic<bool>    m_publisher_thread_exists = false; //
                            std::atomic<bool>    m_reader_thread_exists = false;    //
                            std::recursive_mutex m_publisher_mutex = {};            //
                            std::recursive_mutex m_reader_mutex = {};               //
        )*/
};
} // namespace vengine
