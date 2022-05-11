#pragma once

/*
for loading you must specify the meshname_meshtype_version for the parser to work properly right now
*/

#include "GL/glew.h"

#include <assert.h>
#include <cmath>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//
#include "../Logger.hpp"

namespace vengine
{

namespace mesh
{
using Range = std::pair<unsigned int, unsigned int>;

struct Mesh
{
        Range lod_mesh_ids = {};
        Range bv_ids = {};
};

enum class MeshType : unsigned int
{
    LOD_MESH = 0,        // signifies lods of mesh_states. -> lod
    BOUNDING_VOLUME = 1, // signifies a bounding volume. -> bv
    UNDEF_TYPE = 2       // signifies no type was found
};

struct MeshNodeData
{
        std::string  name = {};
        MeshType     type = {};
        unsigned int version = 0;
        aiMesh*      mesh_ptr = nullptr;
};

struct LoadMeshInfo
{
        // TODO
        bool TODO = false;
};

}; // namespace mesh

class MeshManager
{
    public:

        enum class MeshVertexType : unsigned int
        {
            VERT = 3,
            VERT_NORMAL = 6,
            VERT_TEXTURE = 5,
            VERT_NORMAL_TEXTURE = 8
        };

        MeshManager();
        ~MeshManager() = default;

        void add_meshes(mesh::Range& meshIdRange, const std::vector<std::string>& paths);

        // some other functions

        inline static void set_vertex_attrib_ptrs(MeshManager::MeshVertexType& meshType)
        {
            using MM = MeshManager::MeshVertexType;

            switch (meshType)
            {
                case MM::VERT_NORMAL_TEXTURE:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);                   // vertexs
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3)); // normals
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6)); // normals
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    return;

                case MM::VERT_NORMAL:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                   // vertexs
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3)); // normals
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    return;

                case MM::VERT_TEXTURE:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);                   // vertexs
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3)); // normals
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    return;

                case MM::VERT:
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // vertexs
                    glEnableVertexAttribArray(0);
                    return;

                default:
                    VE_LOG_ERROR("Error, cannot set vertex attrib pointer asthere is an unknown meshType.");
                    return;
            }
        }

        // Getters
        unsigned int get_mesh_id(const std::string& name)
        {
            assert(m_string_to_mesh_index.find(name) != m_string_to_mesh_index.end() &&
                   "ERROR: cannot find a meshIndex with that std::string name.");

            return m_string_to_mesh_index.at(name);
        }

        mesh::Mesh& get_mesh(const std::string& name)
        {
            return get_mesh(get_mesh_id(name));
        }

        mesh::Mesh& get_mesh(unsigned int meshId)
        {
            assert(m_meshes.size() > meshId && "Error: incorrect meshId for getMesh().");
            return m_meshes.at(meshId);
        }

        MeshVertexType& get_mesh_vertex_type(unsigned int lodMeshId)
        {
            assert(m_mesh_vertex_types.size() > lodMeshId && "Error: incorrect lodMeshId for getMeshVertexType().");
            return m_mesh_vertex_types.at(lodMeshId);
        }

        mesh::Range& get_vertex_positions(unsigned int lodMeshId)
        {
            assert(m_mesh_vertex_data_positions.size() > lodMeshId && "Error: incorrect lodMeshId for getVertexDataPositions().");
            return m_mesh_vertex_data_positions.at(lodMeshId);
        }

        mesh::Range& get_index_positions(unsigned int lodMeshId)
        {
            assert(m_mesh_index_data_positions.size() > lodMeshId && "Error: incorrect lodMeshId for getIndexDataPositions().");
            return m_mesh_index_data_positions.at(lodMeshId);
        }

        mesh::Range& get_boundingV_vertex_positions(unsigned int boundingVolumeId)
        {
            assert(m_boundingV_vertex_data_positions.size() > boundingVolumeId && "Error: Incorect boundingVolumeId for vertex positions.");
            return m_boundingV_vertex_data_positions.at(boundingVolumeId);
        }

        std::vector<float> get_boundingV_vertex_data(unsigned int boundingVolumeId)
        {
            const auto& range = get_boundingV_vertex_positions(boundingVolumeId);
            return std::vector<float>(m_boundingV_vertex_data.begin() + range.first, m_boundingV_vertex_data.begin() + range.second + 1);
        }

        std::vector<unsigned int>& get_index_data()
        {
            return m_mesh_index_data;
        }

        std::vector<float>& get_vertex_data()
        {
            return m_mesh_vertex_data;
        }

        std::vector<float>& get_boundingV_vertex_data()
        {
            return m_boundingV_vertex_data;
        }

    private:

        std::unordered_map<std::string, unsigned int> m_string_to_mesh_index;

        // Lod meshes
        std::vector<mesh::Mesh>     m_meshes = {};
        std::vector<MeshVertexType> m_mesh_vertex_types = {};
        std::vector<mesh::Range>    m_mesh_vertex_data_positions = {};
        std::vector<mesh::Range>    m_mesh_index_data_positions = {};
        std::vector<unsigned int>   m_mesh_index_data = {};
        std::vector<float>          m_mesh_vertex_data = {};

        // Bounding volumes
        std::vector<mesh::Range> m_boundingV_vertex_data_positions = {};
        std::vector<float>       m_boundingV_vertex_data = {};

        // Mesh loading through Assimp
        const unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs;
        Assimp::Importer   importer = Assimp::Importer();
        aiScene            scene = aiScene();

        void load_meshes(const std::string& path, const mesh::LoadMeshInfo& loadMeshInfo);

        void collect_mesh_node_data(aiNode* node, const aiScene* scene, std::vector<mesh::MeshNodeData>& mesh_node_data,
                                    std::map<std::string, unsigned int>& string_to_min_val);

        void process_node_data(const std::string& mesh_file_name, std::vector<mesh::MeshNodeData>& mesh_node_data,
                               std::map<std::string, unsigned int>& string_to_min_val);

        void process_mesh(aiMesh* mesh);
        void process_bounding_volume(aiMesh* mesh);

        // Util
        static const std::string         get_filename(const std::string& path);
        static const mesh::MeshType      get_mesh_type(const std::string& mesh_type_string);
        static const mesh::MeshNodeData  get_mesh_name_data(aiMesh* ai_mesh);
        static const size_t              get_closest_after(size_t closest, const std::vector<size_t>& indexes);
        static const std::vector<size_t> find_last_numbers(const std::string& str);
};

}; // namespace vengine
