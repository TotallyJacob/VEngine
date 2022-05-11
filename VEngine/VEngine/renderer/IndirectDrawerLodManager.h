#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

#include "IndirectDrawerCuller.h"
#include "MeshManager.h"
#include "MultiIndirectDrawer.h"

namespace vengine
{
class IndirectDrawerLodManager
{
    public:

        inline void init(MeshManager* meshManager, IndirectDrawerCuller* indirectDrawerCuller,
                         const MultiIndirectDrawer* multiIndirectDrawer, std::vector<unsigned int>& entityIds,
                         std::vector<unsigned int>& meshIds, std::vector<double>& distanceToCamera)
        {
            this->m_mesh_manager = meshManager;
            this->m_indirect_drawer_culler = indirectDrawerCuller;

            establish_lod(multiIndirectDrawer->get_meshIds(), multiIndirectDrawer->get_mesh_lodId_to_dib_index());
            init_entityId_to_lod_level(entityIds, meshIds, distanceToCamera);
        }


        inline void update_lod_levels_batched(const std::vector<unsigned int>& meshIds, const std::vector<unsigned int>& entityIds,
                                              const std::vector<unsigned int>& ssboIds, const std::vector<double>& distances)
        {


            // Gets a vector of all correct lod levels
            std::vector<unsigned int> oldLodMeshIds;

            std::vector<unsigned int> newLodMeshIds;
            std::vector<unsigned int> ssboIdsToChange;

            for (unsigned int i = 0; i < entityIds.size(); i++)
            {
                const auto& meshId = meshIds.at(i);
                const auto& meshData = m_meshId_to_lod_data.at(meshId);
                const auto& entityId = entityIds.at(i);
                const auto& ssboId = ssboIds.at(i);

                unsigned int  newLodMeshId = get_lod_meshId(meshId, get_correct_lod_level(distances.at(i), meshData));
                unsigned int& entityLodMeshId = m_entityId_to_lod_meshId.at(entityId);

                if (newLodMeshId != entityLodMeshId)
                {
                    oldLodMeshIds.push_back(entityLodMeshId);
                    newLodMeshIds.push_back(newLodMeshId);
                    ssboIdsToChange.push_back(ssboId);

                    entityLodMeshId = newLodMeshId; // Switch them round
                }
            }

            // Actualling doing lod stuff
            m_indirect_drawer_culler->cull_entities_batched(oldLodMeshIds, ssboIdsToChange);
            m_indirect_drawer_culler->uncull_entities_batched(newLodMeshIds, ssboIdsToChange);
        }

        inline std::vector<unsigned int> get_lod_meshIds(const std::vector<unsigned int>& entities)
        {
            std::vector<unsigned int> lodMeshIds;
            lodMeshIds.reserve(entities.size());

            for (const auto e : entities)
            {
                lodMeshIds.push_back(m_entityId_to_lod_meshId.at(e));
            }

            return lodMeshIds;
        }


        inline unsigned int get_lod_meshId(unsigned int entityId)
        {
            return m_entityId_to_lod_meshId.at(entityId);
        }

    private:

        // Lod levels go from 0 (highest res) to (numLodLevels - 1) (lowest res)
        struct MeshLodData
        {
                mesh::Range  lodMeshIds = {};
                unsigned int numLodLevels = 1;
                double       distanceForLowestLodLevel = 120;
                double       alpha = 0;
        };
        std::unordered_map<unsigned int, unsigned int> m_entityId_to_lod_meshId{};
        std::unordered_map<unsigned int, MeshLodData>  m_meshId_to_lod_data{};
        std::unordered_map<unsigned int, unsigned int> m_lod_meshId_to_dibId{};

        MeshManager*          m_mesh_manager = nullptr;
        IndirectDrawerCuller* m_indirect_drawer_culler = nullptr;


        // Lod stuff
        inline unsigned int get_lod_meshId(unsigned int meshId, unsigned int lodLevel)
        {
            auto firstLodMeshId = m_meshId_to_lod_data.at(meshId).lodMeshIds.first;
            return (lodLevel + firstLodMeshId);
        }
        inline unsigned int get_correct_lod_level(double distance, const MeshLodData& lodData)
        {
            distance = abs(distance);

            if (distance >= lodData.distanceForLowestLodLevel)
            {
                return lodData.numLodLevels - 1;
            }

            unsigned int lodLevel = std::round(lodData.alpha * distance);

            return (unsigned int)lodLevel; // High for small distances, low for Big distances
        }

        // Init stuff
        inline void init_entityId_to_lod_level(std::vector<unsigned int>& entityIds, std::vector<unsigned int>& meshIds,
                                               std::vector<double>& distanceToCamera)
        {

            for (int i = 0; i < entityIds.size(); i++)
            {
                auto& entityId = entityIds.at(i);
                auto& meshId = meshIds.at(i);
                auto& lodData = m_meshId_to_lod_data.at(meshId);
                auto& distance = distanceToCamera.at(i);


                const unsigned int correctLodLevel = get_correct_lod_level(distance, lodData);
                const unsigned int lodMeshId = get_lod_meshId(meshId, correctLodLevel);
                m_entityId_to_lod_meshId.emplace(entityId, lodMeshId);
            }
        }
        inline void establish_lod(const std::vector<unsigned int>&                      meshIds,
                                  const std::unordered_map<unsigned int, unsigned int>& meshLodIdToDibIndex)
        {

            for (const auto& meshId : meshIds)
            {

                const auto&  mesh = m_mesh_manager->get_mesh(meshId);
                unsigned int numLodLevels = mesh.lod_mesh_ids.second - mesh.lod_mesh_ids.first + 1;

                // Lod data
                MeshLodData lodData = {};
                lodData.numLodLevels = numLodLevels;
                lodData.lodMeshIds = {mesh.lod_mesh_ids.first, mesh.lod_mesh_ids.second};
                lodData.alpha = (lodData.numLodLevels - 1) / (lodData.distanceForLowestLodLevel);
                m_meshId_to_lod_data.emplace(meshId, lodData);

                // Lod mesh ID to dib id
                for (int i = mesh.lod_mesh_ids.first; i <= mesh.lod_mesh_ids.second; i++)
                {
                    auto  lodMeshId = i;
                    auto& dibIndex = meshLodIdToDibIndex.at(lodMeshId);
                    m_lod_meshId_to_dibId.emplace(lodMeshId, dibIndex);
                }
            }
        }
};
}; // namespace vengine
