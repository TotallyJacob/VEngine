#pragma once

namespace vengine{

	    template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers>
        [[nodiscard]] MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>&
        ShaderStorageManager::make_mutable_shader_storage(const std::string name_in_shader, unsigned int num_elements, const SpecialSyncType& special_sync_type)
        {
            using MutableShaderStorageType = MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>;

            VE_DEBUG_DO(shader_storage_creation_assert<DATA_TYPE>(buffer_type, name_in_shader, num_elements));
            const unsigned int current_storage_id = m_shader_storages.size();

            m_shader_storages.push_back(
                std::make_unique<MutableShaderStorageType>(num_elements, true, special_sync_type)
            );

            MutableShaderStorageType& storage = get_mutable_shader_storage<DATA_TYPE, buffer_type, num_buffers>(current_storage_id);

            m_storageId_to_bufferId.emplace(current_storage_id, storage.get_buffer_object());
            m_storage_name_to_storageId.emplace(name_in_shader, current_storage_id);

            increment_shader_storage_bindings(buffer_type);

            return get_mutable_shader_storage<DATA_TYPE, buffer_type, num_buffers>(current_storage_id);

        }

        template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers>
        [[nodiscard]] MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>& ShaderStorageManager::get_mutable_shader_storage(const unsigned int index)
        {
            using MutableShaderStorageType = MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>;
            
            assert(m_shader_storages.size() > index && "Error: m_shader_storages has no shader storage at the speficied index.");

            return dynamic_cast<MutableShaderStorageType&>(*m_shader_storages.at(index));
        }


         template <typename DATA_TYPE>
        void ShaderStorageManager::shader_storage_creation_assert(const GLenum& buffer_type, const std::string& name_in_shader, unsigned int num_elements)
        {
            // Storage id
            assert(m_storage_name_to_storageId.find(name_in_shader) == m_storage_name_to_storageId.end() &&
                   "Error: shaderName is already used by another storageId");

            const size_t size_of_elements = num_elements * sizeof(DATA_TYPE);

            switch (buffer_type)
            {
                case GL_SHADER_STORAGE_BUFFER:

                    assert(m_numUboBindings < m_ssboQuery.maxBufferBindings && "ERROR_UBO: max ubo buffer binding");
                    assert(size_of_elements < (size_t)m_ssboQuery.maxBlockSize && "ERROR_UBO: ubo too large");
                    break;

                case GL_UNIFORM_BUFFER:

                    assert(m_numUboBindings < m_uboQuery.maxBufferBindings && "ERROR_UBO: max ubo buffer binding");
                    assert(size_of_elements < (size_t)m_uboQuery.maxBlockSize && "ERROR_UBO: ubo too large");
                    break;

                default:
                    VE_LOG_ERROR("Cannot create shader storage as buffer_type is not a shader storage buffer type.");
                    std::abort();
                    break;
            }
        }


};