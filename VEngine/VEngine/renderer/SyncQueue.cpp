#include "SyncQueue.h"

using namespace vengine;


SyncQueue::SyncQueue(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
{
    init(num_queues, approximate_num_elements_per_queue);
}

void SyncQueue::init(const unsigned int num_queues, const unsigned int approximate_num_elements_per_queue)
{
    assert(num_queues > 2 && "most be atleast 3 queues to work.");

    SYNC_QUEUE_MUTEX_DO(                                                                                    //
        if (m_reader_license.any_thread_has_license() || m_publisher_license.any_thread_has_license()) {    //
            VE_LOG_ERROR("Cannot init SyncQueue after defining a publisher or reader thread.");             //
            assert(false && "Bad Behaviour; init SyncQueue after defining a publisher or reader thread. "); //
            return;                                                                                         //
        }

        VE_LOG("SyncQueue atomics, lock free?");                                         //
        VE_LOG(lock_free_string("is m_last_read_queue lock free? ", m_last_read_queue)); //
        VE_LOG(lock_free_string("is m_sync_queue_pos lock free? ", m_sync_queue_pos));   //
    );

    m_pre_allocated_num_queue_elements = approximate_num_elements_per_queue;
    m_num_queues = num_queues;

    SyncBuffer buffer;
    buffer.syncs.resize(approximate_num_elements_per_queue);

    for (int i = 0; i < m_num_queues; i++)
    {
        m_sync_queue.emplace(i, buffer);
    }
}

void SyncQueue::publish_queue()
{

    if (!is_publisher_thread())
    {
        return;
    }

    // Maybe add mutex
    unsigned int current_queue_pos = m_sync_queue_pos;
    unsigned int new_queue_pos = (current_queue_pos + 1) % m_num_queues;

    if (new_queue_pos != m_last_read_queue)
    {
        m_sync_queue_pos = new_queue_pos;
    }
}


void SyncQueue::add_sync(const Sync sync)
{
    add_sync_to_queue(sync, m_sync_queue_pos);
}

void SyncQueue::insert_published_syncs_into(std::vector<Sync>& to_insert_into)
{

    if (!is_reader_thread())
    {
        return;
    }

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

void SyncQueue::add_sync_to_queue(const Sync sync, const unsigned int queue_pos)
{
    if (!is_publisher_thread())
    {
        return;
    }

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

void SyncQueue::make_publisher_thread()
{
    SYNC_QUEUE_MUTEX_DO(

        if (!m_publisher_license.try_aquire())                                                  //
        {                                                                                       //
            VE_LOG_ERROR("cannot make publisher thread, another thread is already publisher."); //
            assert(false && "Bad behaviour, only one thread can call this.");                   //
        }                                                                                       //
        else                                                                                    //
        {                                                                                       //
            VE_LOG("SyncQueue, defined a publisher thread");                                    //
        }                                                                                       //

        /* if (!m_publisher_mutex.try_lock())                                                 //
         {                                                                                       //
             VE_LOG_ERROR("cannot make publisher thread, another thread is already publisher."); //
             assert(false && "Bad behaviour, only one thread can call this.");                   //
         }                                                                                       //
         else                                                                                    //
         {                                                                                       //
             VE_LOG("SyncQueue, defined a publisher thread");                                    //
             m_publisher_thread_exists = true;                                                   //
         }                                                                                       //
         */

    );
}
void SyncQueue::make_reader_thread()
{
    SYNC_QUEUE_MUTEX_DO(

        if (!m_reader_license.try_aquire())                                               //
        {                                                                                 //
            VE_LOG_ERROR("cannot make reader thread, another thread is already reader."); //
            assert(false && "Bad behaviour, only one thread can call this.");             //
        }                                                                                 //
        else                                                                              //
        {                                                                                 //
            VE_LOG("SyncQueue, defined a reader thread.");                                //
        }                                                                                 //

        /* if (!m_reader_mutex.try_lock())                                              //
        {                                                                                 //
            VE_LOG_ERROR("cannot make reader thread, another thread is already reader."); //
            assert(false && "Bad behaviour, only one thread can call this.");             //
        }                                                                                 //
        else                                                                              //
        {                                                                                 //
            VE_LOG("SyncQueue, defined a reader thread.");                                //
            m_reader_thread_exists = true;                                                //
        }                                                                                 //
        */
    );
}

auto SyncQueue::is_publisher_thread() -> bool
{
    SYNC_QUEUE_MUTEX_DO( //

        if (!m_publisher_license.thread_has_license())                                                       //
        {                                                                                                    //
            VE_LOG_ERROR("Is not publisher thread, cannot call publish_queue.");                             //
            assert(false && "Bad behaviour, only publisher thread can do this. See make_publisher_thread."); //
            return false;                                                                                    //
        }                                                                                                    //
        else                                                                                                 //
        {                                                                                                    //
            return true;
        } //

        /*
            if (!m_publisher_thread_exists)                                                                 //
            {                                                                                               //
                VE_LOG_ERROR("no publisher thread defined");                                                //
                assert(false && "Bad behaviour, must define a publisher thread -> make_publisher_thread."); //
                return false;                                                                               //
            }                                                                                               //

            std::unique_lock<std::recursive_mutex>       //
                ul(m_publisher_mutex, std::try_to_lock); //

            if (ul.owns_lock()) //
            {                   //
                return true;    //
            }                   //

            VE_LOG_ERROR("Is not publisher thread, cannot call publish_queue.");                             //
            assert(false && "Bad behaviour, only publisher thread can do this. See make_publisher_thread."); //
            return false;                                                                                    // */
    );
    return true;
}

auto SyncQueue::is_reader_thread() -> bool
{
    SYNC_QUEUE_MUTEX_DO( //

        if (!m_reader_license.thread_has_license())                                                    //
        {                                                                                              //
            VE_LOG_ERROR("Is not reader thread, cannot call insert_published_queue.");                 //
            assert(false && "Bad behaviour, only reader thread can do this. See make_reader_thread."); //
            return false;                                                                              //
        }                                                                                              //
        else                                                                                           //
        {                                                                                              //
            return true;
        } //


        /* if (!m_reader_thread_exists)                                                         //
{                                                                                         //
VE_LOG_ERROR("no reader thread defined");                                             //
assert(false && "Bad behaviour, must define a reader thread -> make_reader_thread."); //
return false;                                                                         //
}                                                                                         //

std::unique_lock<std::recursive_mutex>    //
ul(m_reader_mutex, std::try_to_lock); //

if (ul.owns_lock()) //
{                   //
return true;    //
}                   //

VE_LOG_ERROR("Is not reader thread, cannot call insert_published_queue.");                 //
assert(false && "Bad behaviour, only reader thread can do this. See make_reader_thread."); //
return false;                                                                              // */
    );
    return true;
}

#undef SYNC_QUEUE_MUTEX_DO