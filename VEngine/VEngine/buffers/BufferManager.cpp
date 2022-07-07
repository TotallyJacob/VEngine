#pragma once

#include "BufferManager.h"

using namespace buf;

BufferManager::BufferManager()
{
    for (BufferId i = 0; i < prealloc_buffer_ids; i++)
    {
        m_avalible_buffer_ids.push(i);
    }

    m_signatures.resize(prealloc_buffer_ids);
}

[[nodiscard]] BufferId BufferManager::create_buffer()
{
    BufferId id = 0;

    if (m_avalible_buffer_ids.size() != 0) // When we can reuse ids
    {
        id = m_avalible_buffer_ids.front();
        m_avalible_buffer_ids.pop();
    }
    else // When we have to create a new id
    {
        id = m_new_largest_buffer_id;
    }

    m_new_largest_buffer_id++;

    return id;
}

void BufferManager::destroy_buffer(BufferId id)
{
    if (m_new_largest_buffer_id <= id)
    {
        VE_LOG_ERROR("Buffer id too large, cannot destroy buffer. " << __FILE__ << " at " << __LINE__);
        return;
    }

    m_avalible_buffer_ids.push(id);
}

void BufferManager::set_signature(BufferId id, Signature signature)
{
    if (m_signatures.size() <= id)
    {
        size_t new_size = id;
        m_signatures.resize(new_size + 1);
    }

    m_signatures.at(id) = signature;
}

[[nodiscard]] Signature BufferManager::get_signature(BufferId id)
{
    if (m_signatures.size() <= id)
    {
        VE_LOG_ERROR("Buffer id too large, cannot return signature. " << __FILE__ << " at " << __LINE__);
        std::abort();
    }

    return m_signatures.at(id);
}