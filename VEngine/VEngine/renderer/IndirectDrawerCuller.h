#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "MeshManager.h"
#include "MultiIndirectDrawer.h"
#include "MutableDib.h"
#include "MutableShaderStorage.h"

namespace vengine
{

class IndirectDrawerCuller
{
    public:

        IndirectDrawerCuller() = default;


        inline void place_fence()
        {
            m_dib_to_entity_data->insert_sync_on_readbuf();
        }


        inline void swap_buffers()
        {
            // Syncing
            GLsync sync = m_dib_to_entity_data->get_readbuf_sync();
            m_dib_to_entity_data->standard_wait_sync<true, false>(sync, "indirect drawer culler ");

            // Doing everything elsed
            m_dib_to_entity_data->swap_buffers<true>();
            m_dib_to_entity_data->update_readbuf_binding_data();

            m_dib_to_entity_data->copy_previous_updatebuf_into_updatebuf();
            m_dib_to_entity_data->delete_readbuf_sync();
        }

        // Setting stuff
        inline void init(MutableDib* dib, const std::unordered_map<unsigned int, unsigned int>& meshLodIdToDibIndex)
        {
            set_mutable_dib(dib);
            set_mesh_lodId_to_dib_index(meshLodIdToDibIndex);
            set_num_objects();
        }
        inline void set_mutable_dib(MutableDib* dib)
        {
            this->m_mutable_dib = dib;
        }
        inline void set_mesh_lodId_to_dib_index(const std::unordered_map<unsigned int, unsigned int>& meshLodIdToDibIndex)
        {
            this->m_mesh_lod_to_dib_index = meshLodIdToDibIndex;
        }
        inline void set_num_objects()
        {
            m_num_objects = m_mutable_dib->get_num_elements();

            // Init dibInstanceIdToEntityId
            m_dib_instanceId_to_ssboId.resize(m_num_objects);
        }

        // Adding stuff
        void add_entity(unsigned int lodMeshId, unsigned int ssboId);
        void init_ssbo(ShaderStorageManager* shaderStorageManager);
        void set_ssbo(unsigned int lodMeshId, unsigned int ssboId);

        // Culling
        void change_instance_count(unsigned int dibIndex, int byHowMuch);
        void change_instance_count(unsigned int& _numInstances, unsigned int dibIndex, int byHowMuch);

        void uncull_entities_batched(const std::vector<unsigned int>& lodMeshIds, const std::vector<unsigned int>& ssboIds);
        void uncull_entities(unsigned int lodMeshId, unsigned int ssboId);

        void cull_entities_batched(const std::vector<unsigned int>& lodMeshIds, const std::vector<unsigned int>& ssboIds);
        void cull_entities(unsigned int lodMeshId, unsigned int ssboId);

    private:

        unsigned int m_num_objects = 0;
        MutableDib*  m_mutable_dib = nullptr;

        // This shader storage copies and thereby does not change the binding info....
        MutableShaderStorage<unsigned int, GL_SHADER_STORAGE_BUFFER, 3>* m_dib_to_entity_data;
        std::vector<std::unordered_map<unsigned int, unsigned int>>      m_dib_instanceId_to_ssboId{};
        std::unordered_map<unsigned int, unsigned int>                   m_mesh_lod_to_dib_index{};
        std::vector<unsigned int>                                        m_ssboId_to_instanceId{};
};

}; // namespace vengine
