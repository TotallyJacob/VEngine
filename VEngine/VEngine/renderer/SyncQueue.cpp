#include "SyncQueue.h"

using namespace vengine;


SyncQueue::SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
{
    init(num_queues, approximate_num_elements_per_queue);
}

void SyncQueue::init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
{
    assert(num_queues > 2 && "most be atleast 3 queues to work.");

    VE_LOG("SyncQueue atomics, lock free?");
    VE_LOG(lock_free_string("is m_last_read_queue lock free? ", m_last_read_queue));
    VE_LOG(lock_free_string("is m_sync_queue_pos lock free? ", m_sync_queue_pos));

    m_pre_allocated_num_queue_elements = approximate_num_elements_per_queue;
    m_num_queues = num_queues;

    SyncBuffer buffer;
    buffer.syncs.resize(approximate_num_elements_per_queue);

    for (int i = 0; i < m_num_queues; i++)
    {
        m_sync_queue.emplace(i, buffer);
    }
}

// queue

void SyncQueue::publish_queue()
{
    // Maybe add mutex
    unsigned int current_queue_pos = m_sync_queue_pos;
    unsigned int new_queue_pos = (current_queue_pos + 1) % m_num_queues;

    if (new_queue_pos != m_last_read_queue)
    {
        m_sync_queue_pos = new_queue_pos;
    }
}


void SyncQueue::add_sync(const GLsync sync)
{
    add_sync_to_queue(sync, m_sync_queue_pos);
}

void SyncQueue::insert_published_syncs_into(std::vector<GLsync>& to_insert_into)
{
    unsigned int queue_increment = m_last_read_queue;
    unsigned int sync_queue_pos = m_sync_queue_pos;

    if (queue_increment == sync_queue_pos)
    {
        return;
    }

    unsigned int queue_read_up_to = queue_increment;
    while (queue_increment != sync_queue_pos)
    {
        if (queue_increment == sync_queue_pos)
        {
            VE_LOG_ERROR("shouldn't happen");
            break;
        }

        auto&       queue = m_sync_queue.at(queue_increment);
        const auto& buffer = queue.syncs;
        const auto  syncs_to_add = queue.num_added_syncs;

        to_insert_into.insert(to_insert_into.end(), buffer.begin(), buffer.begin() + syncs_to_add);

        queue.num_added_syncs = 0;


        queue_read_up_to = queue_increment;
        queue_increment = (queue_increment + 1) % m_num_queues;
    }

    m_last_read_queue = queue_read_up_to;

    if (m_last_read_queue == sync_queue_pos)

    {
        VE_LOG_ERROR("SyncQueue: somehow we read the queue currently being written too in insert_published_queues.");
    }
}

auto SyncQueue::num_queues() -> unsigned int
{
    return m_num_queues;
}

auto SyncQueue::pre_allocated_num_queue_elements() -> unsigned int
{
    return m_pre_allocated_num_queue_elements;
}

void SyncQueue::add_sync_to_queue(const GLsync sync, const unsigned int queue_pos)
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

auto SyncQueue::lock_free_string(std::string initial_comment, std::atomic<unsigned int>& mutex) -> std::string
{
    initial_comment += (mutex.is_lock_free()) ? " yes" : " no";
    initial_comment += " . is it always lock free?";
    initial_comment += (mutex.is_always_lock_free) ? " yes" : " no";

    return initial_comment;
}