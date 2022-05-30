#pragma once

#include "MutableBuffer.h"

namespace vengine
{

struct IndirectElements
{
        unsigned int count = 0;         // vertices to be rendered
        unsigned int prim_count = 0;    // instances to be rendererd (0 or 1)
        unsigned int first_index = 0;   // position of first index
        unsigned int base_vertex = 0;   // position of first vertex
        unsigned int base_instance = 0; // Used for spliting instances between draw calls apparently.
};


template <const unsigned int num_buffers, const MutableBufferFlags flags = MutableBufferFlags{}>
class MutableDib : public MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, num_buffers, flags>
{
    public:

        MutableDib() = default;

        MutableDib(unsigned int num_elements, bool gen, SpecialSyncType sync_type = SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC)
            : MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, num_buffers, flags>(num_elements, gen, sync_type)
        {
        }

        // specified in glDraw...Indirect -> const void* indirect -> to apply this offset
        const GLintptr get_indirect_offset() const
        {
            return this->m_readbuf_binding_data->offset;
        }
};

} // namespace vengine
