#include "ShaderStorageManager.h"

using namespace vengine;

ShaderStorageManager::ShaderStorageManager()
{
    query_gpu_storage_specs();
}

void ShaderStorageManager::increment_shader_storage_bindings(const GLenum& buffer_type)
{
    switch (buffer_type)
    {
        case GL_SHADER_STORAGE_BUFFER:
            m_numSsboBindings++;
            break;

        case GL_UNIFORM_BUFFER:
            m_numUboBindings++;
            break;

        default:
            VE_LOG_ERROR("Cannot increment the shader_storage_bindings, as buffer_type is not a shader storage buffer.");
            std::abort();
            break;
    }
}


void ShaderStorageManager::query_gpu_storage_specs()
{

    // ubo query
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &m_uboQuery.maxBufferBindings);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &m_uboQuery.maxBlockSize);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &m_uboQuery.maxVertexBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &m_uboQuery.maxFragmentBlocks);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &m_uboQuery.maxGeometryBlocks);


    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &m_ssboQuery.maxBufferBindings);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &m_ssboQuery.maxBlockSize);
    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &m_ssboQuery.maxVertexBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &m_ssboQuery.maxFragmentBlocks);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &m_ssboQuery.maxGeometryBlocks);

    m_uboQuery.printData();
    m_ssboQuery.printData();
}

// Getters
const IMutableBuffer& ShaderStorageManager::get_mutable_shader_storage_interface(const unsigned int storageId) const
{
    assert(m_shader_storages.size() > storageId && "Error, no shader storage at storageId index.");

    return *m_shader_storages.at(storageId);
}
