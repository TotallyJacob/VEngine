#pragma once

namespace vengine
{

/*
    INIT STUFF
*/

MUTABLE_BUFFER_TEMPLATE
MutableBuffer MUTABLE_BUFFER_IDENTIFIER::MutableBuffer(unsigned int num_elements, bool gen, const SpecialSyncType specialSyncType)
{

    assert(1 <= num_buffers && "Number of buffers for a mutable buffer is invalid.");

    if (gen)
    {
        this->gen(num_elements, specialSyncType);
    }
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::gen(unsigned int num_elements, const SpecialSyncType specialSyncType)
{
    size_t     sizeof_elements_per_buf = num_elements * sizeof(DATA_TYPE);
    size_t     alignment_bytes = calculate_alignment_per_buffer(num_elements);
    GLsizeiptr total_buffer_size = static_cast<GLsizeiptr>((sizeof_elements_per_buf + alignment_bytes) * num_buffers);

    constexpr GLbitfield one = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT;

    // Buffer stuff
    glGenBuffers(1, &m_buffer_object);
    bind();
    util::buffer_storage_with_alignment(buffer_type, total_buffer_size, nullptr, get_storage_flags());
    char* m_persistent_map_ptr = util::map_buffer_range_with_alignment(buffer_type, 0, total_buffer_size, get_map_flags());
    unbind();
    m_persistent_maps.init(m_persistent_map_ptr, num_elements, alignment_bytes);

    // Sync stuff
    init_sync_type(specialSyncType);
    for (int i = 0; i < num_buffers; i++)
    {
        m_fences[i] = m_invalid_sync_object; // Thereby glDeleteSync silently ignroes it.
    }

    init_binding_data();
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::init_sync_type(const SpecialSyncType specialSyncType)
{
    if constexpr (num_buffers > 1)
    {
        switch (specialSyncType)
        {
            case SpecialSyncType::NORMAL_SYNC: // read buffer becomes update buffer next frame
                m_readbuf_index = 1;
                m_updatebuf_index = 0;
                m_previous_updatebuf_index = 0;
                break;
            case SpecialSyncType::DOUBLE_BUFFER_SPECIAL_SYNC: // read buffer becomes update buffer next frame unless there are more then 2
                                                              // buffers
                m_readbuf_index = (num_buffers > 2) ? num_buffers - 1 : 1;
                m_updatebuf_index = 0;
                m_previous_updatebuf_index = 0;
                break;
            case SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC: // read buffer becomes update buffer next frame unless there are more then 3
                                                               // buffers
                m_readbuf_index = (num_buffers > 3) ? num_buffers - 2 : 1;
                m_updatebuf_index = 0;
                m_previous_updatebuf_index = 0;
                break;
        }
    }
}


MUTABLE_BUFFER_TEMPLATE inline auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::calculate_alignment_per_buffer(const unsigned int num_elements)
    -> size_t
{
    size_t alignment_per_buffer = 0;

    if constexpr (num_buffers > 1)
    {
        // query the alignment
        GLint alignment = 0;
        switch (buffer_type)
        {
            case GL_SHADER_STORAGE_BUFFER:
                glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);
                break;

            case GL_UNIFORM_BUFFER:
                glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
                break;
            default:
                alignment = -1;
                break;
        }

        // calculate the alignment
        if (alignment > 0)
        {
            // float casts
            float sizeof_data = static_cast<float>(sizeof(DATA_TYPE) * num_elements);
            float alignment_f = static_cast<float>(alignment);
            alignment_per_buffer = ceil(sizeof_data / alignment_f) * alignment_f - sizeof_data;
        }
    }

    return alignment_per_buffer;
}

/*
    BUFFER BINDING STUFF
*/

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::init_binding_data()
{
    m_buffer_size = get_sizeof_elements_per_buf();

    for (unsigned int i = 0; i < num_buffers; i++)
    {
        BindingData& bbd = m_binding_data[i];

        bbd.binding_data_changed = true;
        bbd.offset = (get_alignment_per_buffer() + get_sizeof_elements_per_buf()) * i;
    }

    update_readbuf_binding_data();
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::update_readbuf_binding_data()
{
    m_readbuf_binding_data = &m_binding_data[m_readbuf_index];
}


MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::flush_previous_update_buf()
{
    const BindingData bbd = m_binding_data[m_previous_updatebuf_index];

    bind();

    glFlushMappedBufferRange(buffer_type, bbd.offset, m_buffer_size);

    unbind();
}


/*
    BINDING / BUFFER STUFF
*/

MUTABLE_BUFFER_TEMPLATE
template <bool delete_readbuf_sync>
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::swap_buffers()
{
    if constexpr (num_buffers > 1)
    {
        // Update index
        m_previous_updatebuf_index = m_updatebuf_index;
        m_updatebuf_index = (m_updatebuf_index + 1) % num_buffers;
        m_readbuf_index = (m_readbuf_index + 1) % num_buffers;

        update_readbuf_binding_data();
    }

    if constexpr (flags.explicit_flush)
    {
        flush_previous_update_buf();
    }

    if constexpr (delete_readbuf_sync)
    {
        this->delete_readbuf_sync();
    }
}

MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::delete_buffers()
{
    glDeleteBuffers(num_buffers, &m_buffer_object);
}
MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::bind() const
{
    glBindBuffer(buffer_type, m_buffer_object);
}
MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::unbind() const
{
    glBindBuffer(buffer_type, 0);
}

/*
    SYNC STUFF
*/

MUTABLE_BUFFER_TEMPLATE
template <bool print_if_sync_fail, bool print_sync_time>
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::standard_wait_sync(const GLsync& sync, const std::string& print_sync_message,
                                                                 const GLuint64 wait_time)
{
    // Timer init
    LARGE_INTEGER initial_time;
    if constexpr (print_sync_time)
    {
        Timer timer;
        timer.currentTime(&initial_time);
    }

    // Syncing
    bool   con = false;
    GLenum result;
    while (!con)
    {
        result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, wait_time);

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
    }

    // Printing if sync failed
    if (print_if_sync_fail && result == GL_WAIT_FAILED)
    {
        VE_LOG_DEBUG(print_sync_message << "Not synced.");
    }

    // Time final
    if constexpr (print_sync_time)
    {
        Timer  timer;
        double elapsed = 0;
        timer.getMsElapsed(initial_time, elapsed);

        VE_LOG_DEBUG(print_sync_message << "sync time: " << elapsed << " ms.");
    }
}

MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::delete_fence_syncs()
{
    for (int i = 0; i < num_buffers; i++)
    {
        const GLsync& fenceSync = m_fences[i];
        if (fenceSync != 0)
        {
            glDeleteSync(fenceSync);
        }
    }
}

MUTABLE_BUFFER_TEMPLATE
inline static bool MutableBuffer MUTABLE_BUFFER_IDENTIFIER::sync_invalid(const auto& sync)
{
    return (sync == m_invalid_sync_object);
}

MUTABLE_BUFFER_TEMPLATE
inline bool MutableBuffer MUTABLE_BUFFER_IDENTIFIER::readbuf_sync_invalid()
{
    return sync_invalid(m_fences[m_readbuf_index]);
}

MUTABLE_BUFFER_TEMPLATE
inline bool MutableBuffer MUTABLE_BUFFER_IDENTIFIER::updatebuf_sync_invalid()
{
    return sync_invalid(m_fences[m_updatebuf_index]);
}

MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::delete_readbuf_sync()
{
    auto& sync = m_fences[m_readbuf_index];
    glDeleteSync(m_fences[m_readbuf_index]);
    sync = m_invalid_sync_object; // ensures deleting the sync again after, doesn't end up deleting sync actually in use.
                                  // ensures this deleted sync doesn't interfere with other syncs ever again.
}

MUTABLE_BUFFER_TEMPLATE
inline void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::insert_sync_on_readbuf()
{
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    m_fences[m_readbuf_index] = sync;
}

/*
    FLAG STUFF
*/
MUTABLE_BUFFER_TEMPLATE
constexpr auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_storage_flags() const -> GLbitfield
{
    GLbitfield bf = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

    if (flags.coherent_bit)
    {
        bf |= GL_MAP_COHERENT_BIT;
    }
    if (flags.read_bit)
    {
        bf |= GL_MAP_READ_BIT;
    }
    return bf;
}

MUTABLE_BUFFER_TEMPLATE
constexpr auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_map_flags() const -> GLbitfield
{
    GLbitfield bf = get_storage_flags();
    if (flags.explicit_flush)
    {
        bf |= GL_MAP_FLUSH_EXPLICIT_BIT;
    }
    return bf;
}

/*
    DATA STUFF
*/

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::set_initial_data(unsigned int indexToChange, DATA_TYPE valueToChangeTo)
{
    for (int i = 0; i < num_buffers; i++)
    {
        set_persistent_map_data(i, &valueToChangeTo, 1, indexToChange);
    }
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::set_initial_data(DATA_TYPE* value)
{
    fill_persistent_map(value);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::set_initial_data(DATA_TYPE* values, unsigned int numData)
{
    for (int i = 0; i < num_buffers; i++)
    {
        set_persistent_map_data(i, values, numData, 0);
    }
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::copy_previous_updatebuf_into_updatebuf()
{
    set_updatebuf_data(m_persistent_maps.data[m_previous_updatebuf_index], get_num_elements(), 0);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::insert_data(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                                          unsigned int numElementsToShift)
{
    shift_data(indexToInsertAt, indexToInsertAt + toInsert.size(), numElementsToShift);
    set_updatebuf_data(&toInsert.front(), toInsert.size(), indexToInsertAt);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::insert_data_no_shift(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt)
{

    set_updatebuf_data(&toInsert.front(), toInsert.size(), indexToInsertAt);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::insert_data(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt)
{
    insert_data(toInsert, indexToInsertAt, get_num_elements() - indexToInsertAt);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::insert_data_virtual_max(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                                                      unsigned int virtualMaxNumberOfElements)
{
    insert_data(toInsert, indexToInsertAt, virtualMaxNumberOfElements - indexToInsertAt);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::shift_data(unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                                         unsigned int numElementsToShift)
{
    set_updatebuf_data(&m_persistent_maps.data[m_updatebuf_index][indexToShiftFrom], numElementsToShift, indexToShiftTo);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::shift_data_virtual_max(unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                                                     unsigned int virtualMaxNumberOfElements)
{
    set_updatebuf_data(&m_persistent_maps.data[m_updatebuf_index][indexToShiftFrom], virtualMaxNumberOfElements - indexToShiftFrom,
                       indexToShiftTo);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::set_updatebuf_data(DATA_TYPE* data, unsigned int numOfElements, unsigned int startPoint)
{
    set_persistent_map_data(m_updatebuf_index, data, numOfElements, startPoint);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::fill_persistent_map(DATA_TYPE* value_to_fill)
{
    for (int i = 0; i < num_buffers; i++)
    {
        fill_persistent_map(i, value_to_fill);
    }
}

/*
    ACTUAL MEMCOPY STUFF
*/

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::set_persistent_map_data(const unsigned int index, DATA_TYPE* data, unsigned int numOfElements,
                                                                      unsigned int startPoint)
{
    util::persistent_map_memcpy(m_persistent_maps.data[index], data, numOfElements, startPoint);
}

MUTABLE_BUFFER_TEMPLATE
void MutableBuffer MUTABLE_BUFFER_IDENTIFIER::fill_persistent_map(const unsigned int index, DATA_TYPE* value_to_fill)
{
    util::persistent_map_fill(m_persistent_maps.data[index], value_to_fill, get_num_elements());
}

/*
    GETTERS
*/

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_readbuf_sync() -> GLsync&
{
    return m_fences[m_readbuf_index];
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_updatebuf_sync() -> GLsync&
{
    return m_fences[m_updatebuf_index];
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_byte_array() -> ByteArray<DATA_TYPE, num_buffers>&
{
    return m_persistent_maps;
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_updatebuf_persistent_map(const unsigned int index) -> DATA_TYPE*
{
    return &m_persistent_maps.data[m_updatebuf_index][index];
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_persistent_map(const unsigned int index) -> DATA_TYPE*
{
    return &m_persistent_maps.data[0][index];
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_alignment_per_buffer() const -> const size_t
{
    return m_persistent_maps.get_buffer_alignment();
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_buffer_object() const -> const unsigned int
{
    return m_buffer_object;
}

MUTABLE_BUFFER_TEMPLATE
constexpr auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_buffer_type() const -> const GLenum
{
    return buffer_type;
}

MUTABLE_BUFFER_TEMPLATE
constexpr auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_num_buffers() const -> const unsigned int
{
    return num_buffers;
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_sizeof_elements_per_buf() const -> const size_t
{
    return m_persistent_maps.get_sizeof_elements_per_buf();
}

MUTABLE_BUFFER_TEMPLATE
auto MutableBuffer MUTABLE_BUFFER_IDENTIFIER::get_num_elements() const -> const unsigned int
{
    return m_persistent_maps.get_num_elements_per_buf();
}



}; // namespace vengine