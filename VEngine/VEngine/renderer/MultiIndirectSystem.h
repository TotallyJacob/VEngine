#pragma once

#include "IndirectDrawerCuller.h"
#include "IndirectDrawerLodManager.h"
#include "MultiIndirectDrawer.h"
#include "MutableShaderStorage.h"
#include "ShaderStorageManager.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtx/transform.hpp"

#include "../Timer.h"
#include "../ecs/Coordinator.h"

namespace vengine
{

class MultiIndirectSystem : public System
{
    public:

        struct MultiIndirectData
        {
                unsigned int meshId = 0;
                unsigned int ssboId = 0;
        };

        struct Position
        {
                glm::vec3 position;
        };

        void init(Coordinator* coordinator, MultiIndirectDrawer* drawer)
        {
            this->m_coordinator = coordinator;
            this->m_drawer = drawer;

            // BufferCuller stuff (assume MultiIndirectDrawer is already fully
            // setup)
            m_drawer_culler = std::make_unique<IndirectDrawerCuller>();
            m_drawer_culler->init(drawer->get_mutable_dib(), drawer->get_mesh_lodId_to_dib_index());
            m_lod_manager = std::make_unique<IndirectDrawerLodManager>();

            register_components();
            set_signature();
        }
        void init_lod_manager(MeshManager* meshManager, glm::vec3 cameraPos)
        {

            std::vector<unsigned int> entityIds, meshIds;
            std::vector<double>       distances;
            for (auto& e : m_entities)
            {
                entityIds.push_back(e);
                const auto& data = m_coordinator->get_component<MultiIndirectData>(e);
                meshIds.push_back(data.meshId);

                const auto& pos = m_coordinator->get_component<Position>(e);
                distances.push_back(glm::distance(cameraPos, pos.position));
            }

            m_lod_manager->init(meshManager, m_drawer_culler.get(), m_drawer, entityIds, meshIds, distances);
        }
        void set_textures(unsigned int meshId, unsigned int albedo, unsigned int metallic, unsigned int roughness)
        {
            unsigned int i = m_drawer->get_dib_index(meshId);
            m_albedo_handles->get_persistent_map(i)->handle = albedo;
            m_metallic_handles->get_persistent_map(i)->handle = metallic;
            m_roughness_handles->get_persistent_map(i)->handle = roughness;
        }
        void add_entities_to_renderer(ShaderStorageManager* shaderStorageManager)
        {

            unsigned int ssboId = 0;
            for (auto e : m_entities)
            {
                auto&        data = m_coordinator->get_component<MultiIndirectData>(e);
                unsigned int meshId = m_lod_manager->get_lod_meshId(e);

                m_drawer_culler->add_entity(meshId, e);
                data.ssboId = ssboId;

                ssboId++;

                // Not culled entities
                m_is_entity_culled.emplace(e, false);
            }

            m_drawer_culler->init_ssbo(shaderStorageManager);

            for (auto e : m_entities)
            {
                auto&        data = m_coordinator->get_component<MultiIndirectData>(e);
                unsigned int meshId = m_lod_manager->get_lod_meshId(e);

                m_drawer_culler->set_ssbo(meshId, data.ssboId);
            }

            unsigned int num_meshes = m_drawer->get_num_meshes();
            init_ssbo_data(shaderStorageManager, num_meshes);
        }
        void update_positions()
        {
            m_model_mat_test.resize(m_entities.size());
            m_normal_mat_test.resize(m_entities.size());
            for (auto& e : m_entities)
            {
                auto& ssboPosition = m_coordinator->get_component<MultiIndirectData>(e).ssboId;
                auto& position = m_coordinator->get_component<Position>(e);

                glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), position.position);

                m_model_mat_test.at(ssboPosition) = {modelMat};
                m_normal_mat_test.at(ssboPosition) = {glm::transpose(glm::inverse(modelMat))};
            }
            m_model_mats->set_updatebuf_data(&m_model_mat_test.front(), m_model_mat_test.size(), 0);
            m_normal_matrix->set_updatebuf_data(&m_normal_mat_test.front(), m_normal_mat_test.size(), 0);
        }

        constexpr static unsigned int max = 20000;

        void no_entity()
        {

            LARGE_INTEGER val = {};
            double        elapsed = 0;
            Timer         timer;
            timer.currentTime(&val);

            m_random_entities.assign(m_entities.begin(),
                                     m_entities.end()); // Reduce the size correctly
            std::shuffle(m_random_entities.begin(), m_random_entities.end(), m_rng);
            m_random_entities.resize(max);

            timer.getMsElapsed(val, elapsed);
            std::cout << "Random shuffle: " << elapsed << std::endl;
            timer.currentTime(&val);

            std::vector<unsigned int> entityIds;
            std::vector<unsigned int> ssboIds;
            entityIds.reserve(m_random_entities.size());
            for (auto e : m_random_entities)
            {
                entityIds.push_back(e);
                auto& data = m_coordinator->get_component<MultiIndirectData>(e);
                ssboIds.push_back(data.ssboId);
                m_is_entity_culled.at(e) = true;
            }

            std::vector<unsigned int> meshIds = m_lod_manager->get_lod_meshIds(entityIds);

            m_drawer_culler->cull_entities_batched(meshIds, ssboIds);

            // for (auto e : random_entities) {
            // auto& data = coordinator->getComponent<MultiIndirectData>(e);
            // drawer->removeFromDrawRuntime(data.meshId, e);
            //}

            /*unsigned int i = 0;
            for (auto e : entities) {
                    if (i > 5 && i < 20) {
                            auto& data =
            coordinator->getComponent<MultiIndirectData>(e);
                            meshIds.push_back(data.meshId);
                            entityIds.push_back(e);
                            //drawer->removeFromDrawRuntime(data.meshId, e);
                    }
                    i++;
            }

            drawer->removeFromDrawRuntimeBatched(meshIds, entityIds);*/

            timer.getMsElapsed(val, elapsed);
            std::cout << "Elapsed Remove: " << elapsed << std::endl;
        }
        void yes_entity()
        {
            LARGE_INTEGER val = {};
            double        elapsed = 0;
            Timer         timer;

            timer.currentTime(&val);

            std::vector<unsigned int> entityIds;
            std::vector<unsigned int> ssboIds;
            entityIds.reserve(m_random_entities.size());
            ssboIds.reserve(m_random_entities.size());
            for (auto e : m_random_entities)
            {
                auto& data = m_coordinator->get_component<MultiIndirectData>(e);
                ssboIds.push_back(data.ssboId);
                entityIds.push_back(e);
                m_is_entity_culled.at(e) = false;
            }

            std::vector<unsigned int> meshIds = m_lod_manager->get_lod_meshIds(entityIds);

            m_drawer_culler->uncull_entities_batched(meshIds, ssboIds);

            // for (auto& e : random_entities) {
            // auto& data = coordinator->getComponent<MultiIndirectData>(e);
            // drawer->addFromDrawRuntime(data.meshId, e, data.ssboId);
            //}

            /*unsigned int i = 0;
            for (auto e : entities) {
                    if (i > 5 && i < 20) {
                            auto& data =
            coordinator->getComponent<MultiIndirectData>(e);
                            drawer->addFromDrawRuntime(data.meshId, e,
            data.ssboId);
                    }
                    i++;
            }*/

            timer.getMsElapsed(val, elapsed);
            std::cout << "Elapsed add: " << elapsed << std::endl;
        }
        void update_lods(glm::vec3 cameraPos)
        {

            std::vector<unsigned int> meshIds, entityIds, ssboIds;
            std::vector<double>       distances;
            for (auto& e : m_entities)
            {
                if (!m_is_entity_culled.at(e))
                {
                    const auto& meshData = m_coordinator->get_component<MultiIndirectData>(e);
                    meshIds.push_back(meshData.meshId);
                    entityIds.push_back(e);
                    ssboIds.push_back(meshData.ssboId);

                    const auto pos = m_coordinator->get_component<Position>(e).position;
                    distances.push_back(glm::distance(cameraPos, pos));
                }
            }
            m_lod_manager->update_lod_levels_batched(meshIds, entityIds, ssboIds, distances);
        }
        void play_nice(float dt)
        {
            constexpr static const float angularFrequency = 2 * 3.141592654 * (0.5f);
            for (auto& e : m_entities)
            {
                auto& ssboPosition = m_coordinator->get_component<MultiIndirectData>(e).ssboId;
                auto& position = m_coordinator->get_component<Position>(e);

                glm::vec3 newPos = position.position;
                float     val = newPos.x / 10.f * cos(dt * newPos.x / 120.f * angularFrequency * (1 / 1000.f));
                newPos.y += val;

                m_model_mat_test.at(ssboPosition).mat = glm::translate(glm::mat4(1.0f), newPos);
            }
            m_model_mats->set_updatebuf_data(&m_model_mat_test.front(), m_model_mat_test.size(), 0);
        }
        void place_fence()
        {
            m_drawer_culler->place_fence();
        }
        void swap_buffers()
        {
            m_drawer_culler->swap_buffers();
        }

    private:

        struct Mat4
        {
                glm::mat4 mat;
        };

        struct Texture
        {
                uint64_t handle;
        };

        // Test
        std::mt19937                     m_rng = std::default_random_engine{};
        std::vector<Entity>              m_random_entities;
        std::unordered_map<Entity, bool> m_is_entity_culled{};
        std::vector<Mat4>                m_model_mat_test{};
        std::vector<Mat4>                m_normal_mat_test{};
        //

        std::unique_ptr<IndirectDrawerLodManager> m_lod_manager;
        std::unique_ptr<IndirectDrawerCuller>     m_drawer_culler;

        Coordinator*         m_coordinator = nullptr;
        MultiIndirectDrawer* m_drawer = nullptr;

        MutableShaderStorage<Mat4, GL_SHADER_STORAGE_BUFFER, 1>*    m_normal_matrix;
        MutableShaderStorage<Mat4, GL_SHADER_STORAGE_BUFFER, 1>*    m_model_mats;
        MutableShaderStorage<Texture, GL_SHADER_STORAGE_BUFFER, 1>* m_albedo_handles;
        MutableShaderStorage<Texture, GL_SHADER_STORAGE_BUFFER, 1>* m_metallic_handles;
        MutableShaderStorage<Texture, GL_SHADER_STORAGE_BUFFER, 1>* m_roughness_handles;

        void set_signature()
        {
            Signature sig;
            sig.set(m_coordinator->get_component_type<MultiIndirectData>());
            sig.set(m_coordinator->get_component_type<Position>());
            m_coordinator->set_system_signature<MultiIndirectSystem>(sig);
        }

        void register_components()
        {
            m_coordinator->register_component<MultiIndirectData>();
            m_coordinator->register_component<Position>();
        }

        void init_ssbo_data(ShaderStorageManager* storage_manager, unsigned int num_meshes)
        {
            unsigned int size = m_entities.size();

            m_model_mats = &storage_manager->make_mutable_shader_storage<Mat4, GL_SHADER_STORAGE_BUFFER, 1>("shader_data", size);

            m_normal_matrix = &storage_manager->make_mutable_shader_storage<Mat4, GL_SHADER_STORAGE_BUFFER, 1>("normal_matrix", size);

            m_albedo_handles =
                &storage_manager->make_mutable_shader_storage<Texture, GL_SHADER_STORAGE_BUFFER, 1>("shader_handles", num_meshes);

            m_metallic_handles =
                &storage_manager->make_mutable_shader_storage<Texture, GL_SHADER_STORAGE_BUFFER, 1>("shader_handles2", num_meshes);

            m_roughness_handles =
                &storage_manager->make_mutable_shader_storage<Texture, GL_SHADER_STORAGE_BUFFER, 1>("shader_handles3", num_meshes);
        }
};

}; // namespace vengine
