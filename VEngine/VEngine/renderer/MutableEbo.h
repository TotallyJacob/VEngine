#pragma once

#include "MutableBuffer.h"

namespace vengine
{

template <const unsigned int num_buffers, const MutableBufferFlags flags = MutableBufferFlags{}>
class MutableEbo : public MutableBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, num_buffers, flags>
{
    public:

        MutableEbo() = default;
        MutableEbo(unsigned int num_elements, bool gen, SpecialSyncType sync_type = SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC)
            : MutableBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, num_buffers, flags>(num_elements, gen, sync_type)
        {
        }
        ~MutableEbo() = default;

        // Specified in draw calls or IndirectBuffer -> const void* indices or GLint basevertex or unsigned int first_index -> to apply
        // offset
        auto get_ebo_offset() const -> const GLintptr
        {
            return this->m_readbuf_binding_data->offset;
        }
};

} // namespace vengine
