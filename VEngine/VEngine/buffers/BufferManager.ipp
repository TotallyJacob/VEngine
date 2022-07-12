#pragma once

namespace buf
{

BUFFER_MANAGER_TEMPLATE
BufferManager BUFFER_MANAGER_IDENTIFIER::BufferManager()
{
    for (BufferId i = 0; i < num_buffers; i++)
    {
        m_avalible_buffer_ids.push(i);
    }
}

BUFFER_MANAGER_TEMPLATE
[[nodiscard]] auto BufferManager BUFFER_MANAGER_IDENTIFIER::create_buffer() -> BufferId
{

    assert(m_living_buffer_count < num_buffers && "Too many buffers.");


    BufferId id = m_avalible_buffer_ids.front();
    m_avalible_buffer_ids.pop();

    m_living_buffer_count++;

    return id;
}

BUFFER_MANAGER_TEMPLATE
void BufferManager BUFFER_MANAGER_IDENTIFIER::destroy_buffer(BufferId id)
{
    assert(id < num_buffers && "BufferId too large");

    m_signatures[id].reset();

    m_avalible_buffer_ids.push(id);
    --m_living_buffer_count;
}

BUFFER_MANAGER_TEMPLATE
void BufferManager BUFFER_MANAGER_IDENTIFIER::set_signature(BufferId id, Signature signature)
{
    assert(id < num_buffers && "BufferId too large");

    m_signatures[id] = signature;
}

BUFFER_MANAGER_TEMPLATE
[[nodiscard]] auto BufferManager BUFFER_MANAGER_IDENTIFIER::get_signature(BufferId id) -> Signature
{
    assert(id < num_buffers && "BufferId too large");

    return m_signatures.at(id);
}

} // namespace buf