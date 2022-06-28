namespace vengine
{

CONC_SYNC_BUFFER_TEMPLATE
void ConcSyncBuffer
CONC_SYNC_BUFFER_IDENTIFIER::make_insereter_thread(const std::shared_ptr<ConcSyncInserter<num_buffers, thread_license>>& sync_inserter)
{
    if (m_sync_inserter != nullptr)
    {
        VE_LOG_ERROR("Already a sync inserter for this ConcSyncBuffer.");
        return;
    }

    m_sync_inserter = sync_inserter; // Make the copy
}

CONC_SYNC_BUFFER_TEMPLATE
void ConcSyncBuffer CONC_SYNC_BUFFER_IDENTIFIER::delete_sync_at(unsigned int sync_index)
{
    if (m_sync_inserter == nullptr)
    {
        VE_LOG_ERROR("Sync_Inserter not yet defined, should not call delete_sync_at.");
        return;
    }

    m_sync_inserter.delete_sync_at(sync_index);
}

CONC_SYNC_BUFFER_TEMPLATE
void ConcSyncBuffer CONC_SYNC_BUFFER_IDENTIFIER::sync_at(unsigned int sync_index)->std::atomic<GLsync>&
{
    if (m_sync_inserter == nullptr)
    {
        VE_LOG_ERROR("Sync_Inserter not yet defined, cannot call sync_at.");
        std::abort("Error, sync inserter isnt defined, cannot call sync_at.");
        return {0};
    }

    return m_sync_inserter.sync_at(sync_index);
}

}; // namespace vengine