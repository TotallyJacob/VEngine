#pragma once

#include <assert.h>
#include <bitset>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// test
#include "Logger.hpp"

namespace vengine
{

#define BYTE_ARRAY_IDENTIFIER <DATA_TYPE, num_buffers>
#define BYTE_ARRAY_TEMPLATE   template <typename DATA_TYPE, unsigned int num_buffers>

BYTE_ARRAY_TEMPLATE
class ByteArray
{
    public:

        DATA_TYPE* data[num_buffers] = {nullptr};

        ByteArray();
        ByteArray(char* byte_array, const unsigned int num_elements, const size_t& alignment_bytes);
        void init(char* byte_array, const unsigned int num_elements, const size_t& alignment_bytes);

        ~ByteArray();
        void deconstruct_data();

        // Getters
        auto get_sizeof_elements_per_buf() const -> const size_t;
        auto get_buffer_alignment() const -> const size_t;
        auto get_num_elements_per_buf() const -> const unsigned int;
        auto get_data_bytes() -> char*&;
        auto get_data_bytes() const -> char*;

        // Debug stuff
        void debug_print_all_bytes() const;
        void debug_print_alignment_bytes(const unsigned int buffer) const;
        void debug_print_none_alignment_bytes(const unsigned int buffer) const;

        // Debug getters
        auto get_byte(const unsigned int byte_index) const -> std::bitset<8>;
        auto get_alignment_bytes_start_index(const unsigned int buffer) const -> const unsigned int;
        auto get_none_alignment_bytes_start_index(const unsigned int buffer) const -> const unsigned int;
        auto get_all_alignment_bytes(const unsigned int buffer) const -> const std::vector<std::bitset<8>>;
        auto get_no_alignment_bytes(const unsigned int buffer) const -> const std::vector<std::bitset<8>>;

    private:

        size_t       m_sizeof_elements_per_buf = 0;
        size_t       m_buffer_alignment = 0;
        unsigned int m_num_elements = 0;
        char*        m_data_bytes = nullptr;

        // util
        static void debug_print_bytes(const std::vector<std::bitset<8>>& bytes, const std::string& initial_message = "",
                                      const std::string& message_per_byte = "");

        static auto debug_get_print_bytes_string(const std::vector<std::bitset<8>>& bytes, const std::string& message_per_byte = "")
            -> std::string;
};
}; // namespace vengine

#include "ByteArray.ipp"

#undef BYTE_ARRAY_IDENTIFIER
#undef BYTE_ARRAY_TEMPLATE
