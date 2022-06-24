#include "SyncBuffer.h"

using namespace vengine;


SyncBuffer::SyncBuffer(unsigned int buffer_size)
{
    init(buffer_size);
}

void SyncBuffer::init(unsigned int buffer_size)
{
    m_sync_buf.resize(buffer_size);
}

void SyncBuffer::insert_sync(const unsigned int sync_index)
{
    auto& sync = m_sync_buf.at(sync_index);

    assert(sync == m_invalid_sync && "Sync isn't invalid before trying to insert a new one. Make sure you delete syncs.");

    sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
}

void SyncBuffer::insert_all_syncs()
{
    for (size_t i = 0; i < num_syncs(); i++)
    {
        insert_sync(i);
    }
}

void SyncBuffer::delete_sync(const unsigned int sync_index) // allows an already invalid/deleted sync.
{
    auto& sync = m_sync_buf.at(sync_index);

    if (sync != m_invalid_sync)
    {
        glDeleteSync(m_sync_buf.at(sync_index));
        sync = m_invalid_sync;
    }
}

void SyncBuffer::delete_all_syncs()
{
    for (size_t i = 0; i < num_syncs(); i++)
    {
        delete_sync(i);
    }
}

auto SyncBuffer::sync_at(const unsigned int sync_index) -> GLsync
{
    return m_sync_buf.at(sync_index);
}

auto SyncBuffer::num_syncs() -> const size_t
{
    return m_sync_buf.size();
}

auto SyncBuffer::sync_invalid(const unsigned int sync_index) -> bool
{
    return (m_sync_buf.at(sync_index) == m_invalid_sync);
}