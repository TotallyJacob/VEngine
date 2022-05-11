namespace vengine
{

// Constructors
BYTE_ARRAY_TEMPLATE
ByteArray BYTE_ARRAY_IDENTIFIER ::ByteArray()
{
}

BYTE_ARRAY_TEMPLATE
ByteArray BYTE_ARRAY_IDENTIFIER ::ByteArray(char* byte_array, const unsigned int num_elements, const size_t& alignment_bytes)
{
    init(byte_array, num_elements, alignment_bytes);
}

BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::init(char* byte_array, const unsigned int num_elements, const size_t& alignment_bytes)
{
    m_data_bytes = byte_array;
    m_num_elements = num_elements;
    m_sizeof_elements_per_buf = num_elements * sizeof(DATA_TYPE);
    m_buffer_alignment = alignment_bytes;

    for (unsigned int i = 0; i < num_buffers; i++)
    {
        const size_t alignment_bytes_from_prev_bufs = (sizeof(DATA_TYPE) * num_elements + alignment_bytes) * i;
        new (m_data_bytes + alignment_bytes_from_prev_bufs) DATA_TYPE[num_elements]{};

        data[i] = reinterpret_cast<DATA_TYPE*>(m_data_bytes + alignment_bytes_from_prev_bufs);
    }
}

// Destructor things
BYTE_ARRAY_TEMPLATE
ByteArray BYTE_ARRAY_IDENTIFIER ::~ByteArray()
{
    deconstruct_data();
}

BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::deconstruct_data()
{
    if (data == nullptr || m_data_bytes == nullptr)
    {
        return;
    }

    for (int i = 0; i < num_buffers; i++)
    {
        for (unsigned int j = 0; j < m_num_elements; j++)
        {
            data[i][j].~DATA_TYPE();
        }
    }
}

// Getters
BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_sizeof_elements_per_buf() const -> const size_t
{
    return m_sizeof_elements_per_buf;
}


BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_buffer_alignment() const -> const size_t
{
    return m_buffer_alignment;
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_num_elements_per_buf() const -> const unsigned int
{
    return m_num_elements;
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_data_bytes() -> char*&
{
    return m_data_bytes;
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_data_bytes() const -> char*
{
    return m_data_bytes;
}


// Debug stuff
BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::debug_print_all_bytes() const
{
    std::string whole_print;
    for (int i = 0; i < num_buffers; i++)
    {
        whole_print.append(debug_get_print_bytes_string(get_no_alignment_bytes(i)));
        whole_print.append(debug_get_print_bytes_string(get_all_alignment_bytes(i), "alignment bytes "));
    }
    VE_LOG_DEBUG("printing bytes for all buffers in ByteArrayBuffer: " << '\n' << whole_print);
}

BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::debug_print_alignment_bytes(const unsigned int buffer) const
{
    debug_print_bytes(get_all_alignment_bytes(buffer), "", "alignment bytes ");
}

BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::debug_print_none_alignment_bytes(const unsigned int buffer) const
{
    debug_print_bytes(get_no_alignment_bytes(buffer), "", "none alignment bytes ");
}



// Debug getters
BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_byte(const unsigned int byte_index) const -> std::bitset<8>
{
    const char byte_at_location = m_data_bytes[byte_index];
    return std::bitset<8>(byte_at_location);
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_alignment_bytes_start_index(const unsigned int buffer) const
    -> const unsigned int
{
    return (m_sizeof_elements_per_buf * (buffer + 1)) + (m_buffer_alignment * buffer);
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_none_alignment_bytes_start_index(const unsigned int buffer) const
    -> const unsigned int
{
    return (m_sizeof_elements_per_buf + m_buffer_alignment) * buffer;
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_all_alignment_bytes(const unsigned int buffer) const
    -> const std::vector<std::bitset<8>>
{
    std::vector<std::bitset<8>> bytes;
    bytes.reserve(m_buffer_alignment);

    auto start_location = get_alignment_bytes_start_index(buffer);

    for (int i = start_location; i < start_location + m_buffer_alignment; i++)
    {
        const std::bitset<8> byte = get_byte(i);
        bytes.push_back(byte);
    }

    return bytes;
}

BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::get_no_alignment_bytes(const unsigned int buffer) const
    -> const std::vector<std::bitset<8>>
{

    std::vector<std::bitset<8>> bytes;
    bytes.reserve(m_sizeof_elements_per_buf);

    auto start_location = get_none_alignment_bytes_start_index(buffer);

    for (int i = start_location; i < start_location + m_sizeof_elements_per_buf; i++)
    {
        bytes.push_back(get_byte(i));
    }

    return bytes;
}



// Util
BYTE_ARRAY_TEMPLATE
auto ByteArray BYTE_ARRAY_IDENTIFIER::debug_get_print_bytes_string(const std::vector<std::bitset<8>>& bytes,
                                                                   const std::string&                 message_per_byte) -> std::string
{
    std::string tmp;
    for (const auto& byte : bytes)
    {
        tmp.append(message_per_byte);
        for (int j = 0; j < 8; j++)
        {
            tmp.append(std::to_string(byte[j]));
            tmp.append(" ");
        }
        tmp.append("\n");
    }

    return tmp;
}

BYTE_ARRAY_TEMPLATE
void ByteArray BYTE_ARRAY_IDENTIFIER::debug_print_bytes(const std::vector<std::bitset<8>>& bytes, const std::string& initial_message,
                                                        const std::string& message_per_byte)
{
    VE_LOG_DEBUG_MULTI_START(initial_message);

    VE_LOG_DEBUG_MULTI(debug_get_print_bytes_string(bytes, message_per_byte));

    VE_LOG_DEBUG_MULTI_END();
}

}; // namespace vengine