#pragma once

#include "MutableBuffer.h"

namespace vengine
{

template <typename DATA_TYPE, const GLenum storage_type, unsigned int numBuffers>
class MutableShaderStorage : public MutableBuffer<DATA_TYPE, storage_type, numBuffers>
{

    public:

        MutableShaderStorage() : MutableBuffer<DATA_TYPE, storage_type, numBuffers>()
        {
        }

        MutableShaderStorage(const unsigned int num_elements, const bool gen,
                             const SpecialSyncType specialSyncType = SpecialSyncType::NORMAL_SYNC)
            : MutableBuffer<DATA_TYPE, storage_type, numBuffers>(num_elements, gen, specialSyncType)
        {
            assert((storage_type == GL_SHADER_STORAGE_BUFFER || storage_type == GL_UNIFORM_BUFFER) &&
                   "Error, mutableShaderStorage is neither a GL_SHADER_STORAGE_BUFFER, or GL_UNIFORM_BUFFER.");
        }


        void bind_readbuf_range(const unsigned int base)
        {
            glBindBufferRange(storage_type, base, this->m_buffer_object, this->m_readbuf_binding_data.offset, this->m_buffer_size);
        }

        void bind_readbuf_range(const GLenum storage_type, const GLuint index, const GLuint buffer)
        {
            glBindBufferRange(storage_type, index, buffer, this->m_readbuf_binding_data.offset, this->m_buffer_size);
        }
};

}; // namespace vengine
