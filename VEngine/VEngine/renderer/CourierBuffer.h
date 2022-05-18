#pragma once

#include "../ByteArray.h"

namespace vengine
{

#define COURIER_BUFFER_IDENTIFIER <DATA_TYPE>
#define COURIER_BUFFER_TEMPLATE   template <typename DATA_TYPE>

COURIER_BUFFER_TEMPLATE
class CourierBuffer
{
    public:

        CourierBuffer(unsigned int num_elements);

        auto get_byte_array() -> ByteArray<DATA_TYPE, 1>&;
        auto get_buffer() -> DATA_TYPE*;
        auto get_buf_length() -> unsigned int;

    private:

        ByteArray<DATA_TYPE, 1> m_byte_array = {};
};

} // namespace vengine

#include "CourierBuffer.ipp"

#undef COURIER_BUFFER_IDENTIFIER
#undef COURIER_BUFFER_TEMPLATE