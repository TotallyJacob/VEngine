#pragma once

#include "MutableBuffer.h"

namespace vengine
{

template <const unsigned int num_buffers, const MutableBufferFlags flags = MutableBufferFlags{}>
class MutableVbo : public MutableBuffer<float, GL_ARRAY_BUFFER, num_buffers>
{
    public:


        MutableVbo() = default;
        MutableVbo(unsigned int num_elements, bool gen, SpecialSyncType sync_type = SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC)
            : MutableBuffer<float, GL_ARRAY_BUFFER, num_buffers, flags>(num_elements, gen, sync_type)
        {
        }
        ~MutableVbo() = default;

        // Specified in glBindVertexBuffer -> offset -> to apply this offset
        auto get_vbo_offset() const -> const GLintptr
        {
            return this->m_readbuf_binding_data->offset;
        }
};

} // namespace vengine
