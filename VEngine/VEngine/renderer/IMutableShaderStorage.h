#pragma once

namespace vengine
{

class IMutableShaderStorage
{
    public:

        virtual ~IMutableShaderStorage() = default;

        void bind_readbuf_range(const GLenum storage_type, const GLuint index, const GLuint buffer)
        {
            glBindBufferRange(storage_type, index, buffer, m_readbuf_offset, m_readbuf_size);
        }

        unsigned int& binding_data_changed()
        {
            return m_binding_data_changed;
        }

        GLintptr& get_readbuf_offset()
        {
            return m_readbuf_offset;
        }

        GLsizeiptr& get_readbuf_size()
        {
            return m_readbuf_size;
        }

    protected:

        unsigned int m_binding_data_changed = true;
        GLintptr     m_readbuf_offset = 0;
        GLsizeiptr   m_readbuf_size = 0;
};

}; // namespace vengine
