#pragma once

namespace vengine
{
class IMutableBuffer
{
    public:

        struct BindingData
        {
                unsigned int binding_data_changed = true;
                GLintptr     offset = 0;
        };

        GLsizeiptr   m_buffer_size = 0;
        BindingData* m_readbuf_binding_data = nullptr;

        virtual ~IMutableBuffer() = default;
};

} // namespace vengine
