#pragma once

#include "ByteArray.h"
#include "renderer/PersistentMapBufferUtil.h"

namespace vengine
{

#define COURIER_BUFFER_IDENTIFIER <DATA_TYPE>
#define COURIER_BUFFER_TEMPLATE   template <typename DATA_TYPE>

COURIER_BUFFER_TEMPLATE
class CourierBuffer
{
    public:

        CourierBuffer() = default;
        CourierBuffer(unsigned int num_elements, size_t alignment_bytes);

        void init(unsigned int num_elements, size_t alignment_bytes);
        void set_data(const std::vector<DATA_TYPE>& data);
        void set_data(DATA_TYPE* data, unsigned int num_elements);

        auto get_byte_array() -> ByteArray<DATA_TYPE, 1>&;
        auto get_data(unsigned int index) -> DATA_TYPE*;
        auto get_data() -> DATA_TYPE*;
        auto get_num_elements() -> unsigned int;

    private:

        ByteArray<DATA_TYPE, 1> m_byte_array = {};
        std::vector<char>       m_data = {};
};

} // namespace vengine

#include "CourierBuffer.ipp"

#undef COURIER_BUFFER_IDENTIFIER
#undef COURIER_BUFFER_TEMPLATE