#pragma once

#include "MutableBuffer.h"

namespace vengine
{

class MutableEbo : public MutableBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, 1>
{
    public:

        MutableEbo(unsigned int num_elements, bool gen);
        ~MutableEbo() = default;
};

} // namespace vengine
