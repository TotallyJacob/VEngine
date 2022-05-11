#pragma once

#include "MutableBuffer.h"

namespace vengine
{

class MutableVbo : public MutableBuffer<float, GL_ARRAY_BUFFER, 1>
{
    public:

        MutableVbo(unsigned int num_elements, bool gen);
        ~MutableVbo() = default;
};

} // namespace vengine
