namespace vengine
{

CONC_SYNC_INSERTER_TEMPLATE
ConcSyncInserter CONC_SYNC_INSERTER_IDENTIFIER::ConcSyncInserter(bool init)
{
    if (init)
    {
        this->init();
    }
}

CONC_SYNC_INSERTER_TEMPLATE
void ConcSyncInserter CONC_SYNC_INSERTER_IDENTIFIER::init()
{

    if (!m_inserter_license.try_aquire())
    {
        VE_LOG_ERROR("Cannot make ConcSyncInserter inserter thread in constructor. Huge problem going on.");
    }
}

CONC_SYNC_INSERTER_TEMPLATE
auto ConcSyncInserter CONC_SYNC_INSERTER_IDENTIFIER::try_insert(unsigned int sync_index) -> bool
{
    if (!m_inserter_license.thread_has_license())
    {
        VE_LOG_WARNING("Trying to insert syncs on a thread which doesn't have a thread license.");
        return false;
    }

    auto& m_sync = m_syncs.at(sync_index);


    if (m_previous_sync_index == sync_index)
    {
        return false;
    }

    if (m_sync != m_invalid_sync)
    {
        return false;
    }

    m_previous_sync_index = sync_index;

    m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();

    return true;
}

CONC_SYNC_INSERTER_TEMPLATE
void ConcSyncInserter CONC_SYNC_INSERTER_IDENTIFIER::delete_sync_at(unsigned int sync_index)
{
    auto& sync = m_syncs.at(sync_index);

    if (sync == m_invalid_sync)
    {
        return;
    }

    glDeleteSync(sync);
    sync = m_invalid_sync;
}

CONC_SYNC_INSERTER_TEMPLATE
auto ConcSyncInserter CONC_SYNC_INSERTER_IDENTIFIER::sync_at(unsigned int sync_index) -> std::atomic<GLsync>&
{
    return m_syncs.at(sync_index);
}

}; // namespace vengine