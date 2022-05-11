#pragma once

#include "IMutableShaderStorage.h"
#include "MutableBuffer.h"

namespace vengine
{

template <typename DATA_TYPE, const GLenum storage_type, unsigned int numBuffers>
class MutableShaderStorage : public IMutableShaderStorage, public MutableBuffer<DATA_TYPE, storage_type, numBuffers>
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

            update_readbuf_binding_data();
        }


        void update_readbuf_binding_data()
        {
            this->m_binding_data_changed = true;
            this->m_readbuf_offset = (this->get_alignment_per_buffer() + this->get_sizeof_elements_per_buf()) * this->m_readbuf_index;
            this->m_readbuf_size = this->get_sizeof_elements_per_buf();
        }

        void bind_readbuf_range(const unsigned int base)
        {
            glBindBufferRange(storage_type, base, this->m_buffer_object, this->m_readbuf_offset, this->m_readbuf_size);
        }
};

}; // namespace vengine
