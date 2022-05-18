#pragma once

#include "GL/glew.h"

// My includes
#include "../Logger.hpp"
#include "IMutableBuffer.h"
#include "MutableBuffer.h"
#include "ShaderProgramManager.h"
#include "ShaderStorageBinder.h"

#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtx/transform.hpp"
#include "MutableShaderStorage.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace vengine
{

class ShaderStorageManager
{
    public:

        ShaderStorageManager();
        ~ShaderStorageManager() = default;

        ShaderStorageManager(const ShaderStorageManager&) =
            delete; // Nasty story behind this (was accidently copying with a vector of unieque_ptrs which cant be copied)

        void init_binding_manager(ShaderProgramManager* programManager)
        {
            m_shader_storage_binder =
                std::make_unique<ShaderStorageBinder>(m_ssboQuery.maxBufferBindings, m_uboQuery.maxBufferBindings, m_storageId_to_bufferId,
                                                      m_storage_name_to_storageId, m_shader_storages, *programManager);
        }

        inline void bind_shader_storages(unsigned int programId)
        {
            m_shader_storage_binder->bind_storages(programId);
        }

        const IMutableBuffer& get_mutable_shader_storage_interface(const unsigned int storageId) const;

        template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers>
        [[nodiscard]] MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>&
        make_mutable_shader_storage(const std::string name_in_shader, unsigned int num_elements,
                                    const SpecialSyncType& special_sync_type = SpecialSyncType::NORMAL_SYNC);


        // Getters
        template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers>
        [[nodiscard]] MutableShaderStorage<DATA_TYPE, buffer_type, num_buffers>& get_mutable_shader_storage(const unsigned int storageId);

        inline std::unordered_map<std::string, unsigned int>& get_storage_name_to_storageId_map()
        {
            return m_storage_name_to_storageId;
        }

        inline unsigned int getCurrentStorageId() const
        {
            return m_numSsboBindings + m_numUboBindings;
        }

        inline unsigned int getStorageId(const std::string& shaderName) const
        {
            assert(m_storage_name_to_storageId.find(shaderName) != m_storage_name_to_storageId.end() &&
                   "Error: shaderName has no registered shaderId");

            return m_storage_name_to_storageId.at(shaderName);
        }

    private:

        // Data
        std::unique_ptr<ShaderStorageBinder>           m_shader_storage_binder;
        std::vector<std::unique_ptr<IMutableBuffer>>   m_shader_storages{};
        std::unordered_map<unsigned int, unsigned int> m_storageId_to_bufferId{};
        std::unordered_map<std::string, unsigned int>  m_storage_name_to_storageId{};

        // Query
        struct GPU_QUERY
        {
                int maxBufferBindings = 0;
                int maxBlockSize = 0;
                int maxVertexBlocks = 0;
                int maxFragmentBlocks = 0;
                int maxGeometryBlocks = 0;

                void printData() const
                {
                    VE_LOG_MULTI_START("GPU QUERY: ");
                    VE_LOG_MULTI("GL_MAX_BUFFER_BINDINGS ->" << maxBufferBindings);
                    VE_LOG_MULTI("GL_MAX_BLOCK_SIZE -> " << maxBlockSize);
                    VE_LOG_MULTI("GL_MAX_VERTEX_BLOCKS -> " << maxVertexBlocks);
                    VE_LOG_MULTI("GL_MAX_FRAGMENT_BLOCKS -> " << maxFragmentBlocks);
                    VE_LOG_MULTI("GL_MAX_GEOMETRY_BLOCKS -> " << maxGeometryBlocks);
                    VE_LOG_MULTI_END();
                }
        };
        GPU_QUERY    m_uboQuery = {};
        GPU_QUERY    m_ssboQuery = {};
        unsigned int m_numUboBindings = 0;
        unsigned int m_numSsboBindings = 0;

        template <typename DATA_TYPE>
        void shader_storage_creation_assert(const GLenum& buffer_type, const std::string& name_in_shader, unsigned int num_elements);
        void increment_shader_storage_bindings(const GLenum& buffer_type);
        void query_gpu_storage_specs();
};

} // namespace vengine

#include "ShaderStorageManager.ipp"
