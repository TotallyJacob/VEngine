#include "SyncManager.h"

using namespace vengine;

SyncManager::SyncManager(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size)
{
    init(num_sync_queues, pre_allocated_sync_queue_size);
}

void SyncManager::init(unsigned int num_sync_queues, unsigned int pre_allocated_sync_queue_size)
{
    m_sync_queue.init(num_sync_queues, pre_allocated_sync_queue_size);

    HDC   hdc = wglGetCurrentDC();
    HGLRC hglrc = wglGetCurrentContext();

    m_thread = std::make_unique<std::thread>(&SyncManager::thread_function, this, hdc, hglrc);
}

SyncManager::~SyncManager()
{
    stop();
}

void SyncManager::add_sync(const GLsync sync)
{
    m_sync_queue.add_sync(sync);
}

void SyncManager::stop()
{
    m_running = false;

    if (m_thread->joinable())
    {
        m_thread->join();
        VE_LOG("joined Sync manager thread.");
    }
    else
    {
        VE_LOG_ERROR("sync manager thread isn't joinable().");
    }
}

void SyncManager::thread_sync_handler(GLenum& result)
{
    m_sync_queue.insert_published_syncs_into(m_syncs);

    static const GLsync invalid_sync = 0;

    for (auto& sync : m_syncs)
    {
        // Sleep(0);

        if (sync == invalid_sync)
        {
            continue;
        }

        result = glClientWaitSync(sync, 0, 0);

        bool con = false;
        switch (result)
        {
            case GL_WAIT_FAILED:
                con = true;
                break;

            case GL_CONDITION_SATISFIED:
                con = true;
                break;

            case GL_ALREADY_SIGNALED:
                con = true;
                break;
        }

        if (con)
        {
            VE_LOG_WARNING("con changed, so removing sync: " << sync);
            sync = invalid_sync;
        }
    }

    // Removing invalid sync objects
    auto to_remove = std::find(m_syncs.begin(), m_syncs.end(), invalid_sync);
    if (to_remove != m_syncs.end())
    {
        m_syncs.erase(to_remove);
    }
}
void SyncManager::thread_function(HDC hdc, HGLRC hglrc)
{
    // Windows context stuff
    HGLRC hglrc_new = wglCreateContext(hdc);
    wglShareLists(hglrc, hglrc_new);
    wglMakeCurrent(hdc, hglrc_new);
    VE_LOG("Sync manager, made context.");


    GLenum result;
    while (m_running)
    {
        thread_sync_handler(result);
    }


    // Deleting the context
    VE_LOG("Sync manager, deleting context.");
    wglDeleteContext(hglrc_new);
    VE_LOG("Stopping running sync manager thread.");
}
