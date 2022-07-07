namespace vengine
{

SYNC_MANAGER_TEMPLATE
SyncManager SYNC_MANAGER_IDENTIFIER::SyncManager()
{
    init();
}

SYNC_MANAGER_TEMPLATE
void SyncManager SYNC_MANAGER_IDENTIFIER::init()
{
    m_sync_inserter.init();
    m_syncbuf.make_sync_inserter(m_sync_inserter);

    HDC   hdc = wglGetCurrentDC();
    HGLRC hglrc = wglGetCurrentContext();

    m_thread = std::make_unique<std::thread>(&SyncManager::thread_function, this, hdc, hglrc);
}

SYNC_MANAGER_TEMPLATE
SyncManager SYNC_MANAGER_IDENTIFIER::~SyncManager()
{
    stop();
}


SYNC_MANAGER_TEMPLATE
void SyncManager SYNC_MANAGER_IDENTIFIER::stop()
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

SYNC_MANAGER_TEMPLATE
void SyncManager SYNC_MANAGER_IDENTIFIER::thread_sync_handler(GLenum& result)
{
    /* m_sync_queue.insert_published_syncs_into(m_syncs);

    static const GLsync invalid_sync = 0;

    std::this_thread::sleep_for(std::chrono::microseconds(10));

    for (auto& sync_object : m_syncs)
    {
        auto& sync = sync_object.sync;

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
            const auto& buffer_id = sync_object.buffer_id;
            m_mutable_buffer_syncs_signaled.at(buffer_id) = true;
            VE_LOG_WARNING("con changed, so removing sync: " << sync);
            sync = invalid_sync;
        }
    }

    // Removing invalid sync objects
    // auto to_remove = std::find(m_syncs.begin(), m_syncs.end(), invalid_sync);
    // if (to_remove != m_syncs.end())
    //{
    //  m_syncs.erase(to_remove);
    // } */
}

SYNC_MANAGER_TEMPLATE
void SyncManager SYNC_MANAGER_IDENTIFIER::thread_function(HDC hdc, HGLRC hglrc)
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

}; // namespace vengine