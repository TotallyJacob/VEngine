namespace vengine
{

/*
    INIT STUFF
*/
COURIER_BUFFER_TEMPLATE
CourierBuffer COURIER_BUFFER_IDENTIFIER::CourierBuffer(unsigned int num_elements, size_t alignment_bytes)
{
    init(num_elements, alignment_bytes);
}

COURIER_BUFFER_TEMPLATE
void CourierBuffer COURIER_BUFFER_IDENTIFIER::init(unsigned int num_elements, size_t alignment_bytes)
{
    m_data.resize((num_elements * sizeof(DATA_TYPE)) + alignment_bytes); // Allocate the chars on heap.
    m_byte_array.init(&m_data.front(), num_elements, alignment_bytes);
}

COURIER_BUFFER_TEMPLATE
void CourierBuffer COURIER_BUFFER_IDENTIFIER::set_data(DATA_TYPE* data, unsigned int num_elements)
{
    util::buf_memcpy(get_data(), data, num_elements, 0);
}

COURIER_BUFFER_TEMPLATE
void CourierBuffer COURIER_BUFFER_IDENTIFIER::set_data(const std::vector<DATA_TYPE>& data)
{
    util::buf_memcpy(get_data(), &data.front(), data.size(), 0);
}

/*
    GETTERS
*/

COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_byte_array() -> ByteArray<DATA_TYPE, 1>&
{
    return m_byte_array;
}


COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_data(unsigned int index) -> DATA_TYPE*
{
    assert(index < get_num_elements() && "Index larger then buffer length.");

    return &m_byte_array.data[0][index];
}


COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_data() -> DATA_TYPE*
{
    return m_byte_array.data[0];
}

COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_num_elements() -> unsigned int
{
    return m_byte_array.get_num_elements_per_buf();
}

}; // namespace vengine