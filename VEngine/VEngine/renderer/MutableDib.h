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

class MutableDib : public MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, 1>
{
    public:

        MutableDib(unsigned int num_elements, bool gen);
        ~MutableDib() = default;
};

} // namespace vengine
