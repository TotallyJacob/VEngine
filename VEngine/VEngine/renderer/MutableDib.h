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

constexpr static const MutableBufferFlags mdibf = {true, false, false};

template <const unsigned int num_buffers>
class MutableDib : public MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, num_buffers, mdibf>
{
    public:

        MutableDib() = default;

        MutableDib(unsigned int num_elements, bool gen)
            : MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, num_buffers, mdibf>(num_elements, gen,
                                                                                           SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC)
        {
        }

        const GLintptr get_indirect_offset() const
        {
            return this->m_readbuf_binding_data->offset;
        }
};

} // namespace vengine
