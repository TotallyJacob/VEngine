namespace vengine
{

COURIER_BUFFER_TEMPLATE
CourierBuffer COURIER_BUFFER_IDENTIFIER::CourierBuffer(unsigned int num_elements)
{
}

COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_byte_array() -> ByteArray<DATA_TYPE, 1>&
{
    return m_byte_array;
}

COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_buffer() -> DATA_TYPE*
{
    return m_byte_array.data[0];
}

COURIER_BUFFER_TEMPLATE
auto CourierBuffer COURIER_BUFFER_IDENTIFIER::get_buf_length() -> unsigned int
{
    return m_byte_array.get_num_elements_per_buf();
}

}; // namespace vengine