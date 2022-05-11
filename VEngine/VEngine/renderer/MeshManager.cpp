#include "MeshManager.h"

using namespace vengine;

MeshManager::MeshManager()
{
}

void MeshManager::add_meshes(mesh::Range& meshIdRange, const std::vector<std::string>& paths)
{
    VE_LOG("Adding meshes.");
    meshIdRange.first = m_meshes.size();
    for (int i = 0; i < paths.size(); i++)
    {
        const auto& path = paths.at(i);
        load_meshes(path, {});
    }
    meshIdRange.second = m_meshes.size() - 1;
}

// MeshLoading
void MeshManager::load_meshes(const std::string& path, const mesh::LoadMeshInfo& loadMeshInfo)
{
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        VE_LOG_ERROR(std::string("ASSIMP error: ") + importer.GetErrorString());
        return;
    }

    std::map<std::string, unsigned int> string_to_min_val{};
    std::vector<mesh::MeshNodeData>     mesh_node_data{};
    const std::string                   mesh_filename = get_filename(path);

    collect_mesh_node_data(scene->mRootNode, scene, mesh_node_data, string_to_min_val);
    process_node_data(mesh_filename, mesh_node_data, string_to_min_val);

    importer.FreeScene();
}

void MeshManager::collect_mesh_node_data(aiNode* node, const aiScene* scene, std::vector<mesh::MeshNodeData>& mesh_node_data,
                                         std::map<std::string, unsigned int>& string_to_min_val)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        const auto    meshId = node->mMeshes[i];
        const aiMesh* mesh = scene->mMeshes[meshId];

        // Mesh name data
        const std::string mesh_name = mesh->mName.C_Str();
        auto              _mesh_node_data = get_mesh_name_data(scene->mMeshes[meshId]);

        // Setting string_to_min_val
        const auto name = _mesh_node_data.name;
        const auto val = _mesh_node_data.version;

        // Min value
        if (string_to_min_val.find(name) == string_to_min_val.end())
        {
            string_to_min_val.emplace(name, val);
        }
        else
        {
            auto& min_val = string_to_min_val.at(name);
            if (min_val > val)
            {
                min_val = val;
            }
        }

        mesh_node_data.push_back(_mesh_node_data);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        collect_mesh_node_data(node->mChildren[i], scene, mesh_node_data, string_to_min_val);
    }
}

void MeshManager::process_node_data(const std::string& mesh_file_name, std::vector<mesh::MeshNodeData>& mesh_node_data,
                                    std::map<std::string, unsigned int>& string_to_min_val)
{

    // Sort the data
    std::unordered_map<std::string, std::vector<mesh::MeshNodeData>> string_to_sorted_meshes;
    for (auto& mesh_data : mesh_node_data)
    {
        const auto mesh_name = mesh_data.name;
        if (string_to_sorted_meshes.find(mesh_name) == string_to_sorted_meshes.end())
        {
            std::vector<mesh::MeshNodeData> mesh_data_sorted;
            mesh_data_sorted.push_back(mesh_data);

            string_to_sorted_meshes.emplace(mesh_name, mesh_data_sorted);
            continue;
        }

        const auto min_val = string_to_min_val.at(mesh_name);
        auto&      mesh_data_sorted = string_to_sorted_meshes.at(mesh_name);
        mesh_data_sorted.insert(mesh_data_sorted.begin() + (mesh_data.version - min_val), mesh_data);
    }

    // Parse the messes
    for (auto& pair : string_to_sorted_meshes)
    {
        auto& string = pair.first;
        auto& mesh_node_datas = pair.second;

        mesh::Mesh mesh = {};
        bool       first_lod_mesh = true;
        bool       first_bv = true;
        for (auto& mesh_data : mesh_node_datas)
        {
            switch (mesh_data.type)
            {
                case mesh::MeshType::LOD_MESH:

                    if (first_lod_mesh)
                    {
                        mesh.lod_mesh_ids = {m_mesh_vertex_data_positions.size(), m_mesh_vertex_data_positions.size()};
                        first_lod_mesh = false;
                    }
                    else
                    {
                        mesh.lod_mesh_ids.second = m_mesh_vertex_data_positions.size();
                    }

                    process_mesh(mesh_data.mesh_ptr);
                    break;

                case mesh::MeshType::BOUNDING_VOLUME:


                    if (first_bv)
                    {
                        mesh.bv_ids = {m_boundingV_vertex_data_positions.size(), m_boundingV_vertex_data_positions.size()};
                        first_bv = false;
                    }
                    else
                    {
                        mesh.bv_ids.second = m_boundingV_vertex_data_positions.size();
                    }

                    process_bounding_volume(mesh_data.mesh_ptr);
                    break;

                case mesh::MeshType::UNDEF_TYPE:
                    VE_LOG_WARNING("cannot process undefined mesh type named: " + mesh_data.name);
                    break;
            }
        }
        m_string_to_mesh_index.emplace(mesh_file_name + "." + string, m_meshes.size());
        m_meshes.push_back(mesh);
    }
}

void MeshManager::process_mesh(aiMesh* mesh)
{
    if ((mesh->mNumVertices || mesh->mNumFaces) == 0)
    {
        VE_LOG_ERROR("mesh has no vertices or faces.");
        return;
    }

    // MeshType
    bool hasNormals = mesh->HasNormals();
    bool hasTextureCoords = mesh->HasTextureCoords(0);

    MeshVertexType meshType = {};
    if (hasNormals && hasTextureCoords)
    {
        meshType = MeshVertexType::VERT_NORMAL_TEXTURE;
    }
    if (hasNormals && !hasTextureCoords)
    {
        meshType = MeshVertexType::VERT_NORMAL;
    }
    if (hasTextureCoords && !hasNormals)
    {
        meshType = MeshVertexType::VERT_TEXTURE;
    }

    m_mesh_vertex_types.push_back(meshType);

    // Mesh range data
    mesh::Range meshDataRange = {};

    // VertexData
    meshDataRange.first = m_mesh_vertex_data.size();
    m_mesh_vertex_data.reserve(m_mesh_vertex_data.size() + mesh->mNumVertices * static_cast<unsigned int>(meshType));
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // Verts
        const auto mVertex = mesh->mVertices[i];
        m_mesh_vertex_data.push_back(mVertex.x);
        m_mesh_vertex_data.push_back(mVertex.y);
        m_mesh_vertex_data.push_back(mVertex.z);

        // Normals
        if (hasNormals)
        {
            const auto mNormal = mesh->mNormals[i];
            m_mesh_vertex_data.push_back(mNormal.x);
            m_mesh_vertex_data.push_back(mNormal.y);
            m_mesh_vertex_data.push_back(mNormal.z);
        }

        // Textures
        if (hasTextureCoords)
        {
            const auto mTex = mesh->mTextureCoords[0][i];
            m_mesh_vertex_data.push_back(mTex.x);
            m_mesh_vertex_data.push_back(mTex.y);
        }
    }
    meshDataRange.second = m_mesh_vertex_data.size() - 1;

    m_mesh_vertex_data_positions.push_back(meshDataRange);

    // IndexData
    meshDataRange.first = m_mesh_index_data.size();
    const unsigned int numFaceIndices = mesh->mFaces[0].mNumIndices;
    m_mesh_index_data.reserve(m_mesh_index_data.size() + mesh->mNumFaces * numFaceIndices);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        assert(numFaceIndices == face.mNumIndices &&
               "Error in MeshLoader, number of face indices for first face doesn't equal the number of indices for subsequent faces.");

        for (unsigned int j = 0; j < numFaceIndices; j++)
        {
            m_mesh_index_data.push_back(face.mIndices[j]);
        }
    }
    meshDataRange.second = m_mesh_index_data.size() - 1;

    m_mesh_index_data_positions.push_back(meshDataRange);
}

void MeshManager::process_bounding_volume(aiMesh* mesh)
{
    if ((mesh->mNumVertices || mesh->mNumFaces) == 0)
    {
        VE_LOG_ERROR("bounding volume has no vertices or faces.");
        return;
    }

    // Mesh range data
    mesh::Range bounding_volume_vertex_range = {};

    // VertexData
    std::vector<aiVector3D> temp_vertex_data;
    temp_vertex_data.reserve(mesh->mNumVertices);

    bounding_volume_vertex_range.first = m_boundingV_vertex_data.size();
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        const auto mVertex = mesh->mVertices[i];
        if (std::find(temp_vertex_data.begin(), temp_vertex_data.end(), mVertex) == temp_vertex_data.end())
        {
            temp_vertex_data.push_back(mVertex);
        }
    }

    // Push back none-duplicate vertices to actual data
    for (const auto& vertex : temp_vertex_data)
    {
        m_boundingV_vertex_data.push_back(vertex.x);
        m_boundingV_vertex_data.push_back(vertex.y);
        m_boundingV_vertex_data.push_back(vertex.z);
    }

    bounding_volume_vertex_range.second = m_boundingV_vertex_data.size() - 1;
    m_boundingV_vertex_data_positions.push_back(bounding_volume_vertex_range);
}

// Util
const std::string MeshManager::get_filename(const std::string& path)
{
    const std::string            baseFilename = path.substr(path.find_last_of("/\\") + 1);
    const std::string::size_type lastDot(baseFilename.find_last_of('.'));
    return baseFilename.substr(0, lastDot);
}

const mesh::MeshType MeshManager::get_mesh_type(const std::string& mesh_type_string)
{
    if (mesh_type_string == "lod")
    {
        return mesh::MeshType::LOD_MESH;
    }

    if (mesh_type_string == "bv")
    {
        return mesh::MeshType::BOUNDING_VOLUME;
    }

    return mesh::MeshType::UNDEF_TYPE;
}

const mesh::MeshNodeData MeshManager::get_mesh_name_data(aiMesh* ai_mesh)
{
    const std::string mesh_name(ai_mesh->mName.C_Str());

    size_t first_index = std::string::npos;
    size_t second_index = std::string::npos;

    std::size_t found = mesh_name.find("_", 0);
    if (found != std::string::npos)
    {
        first_index = found;
    }

    found = mesh_name.find("_", first_index + 1);
    if (found != std::string::npos)
    {
        second_index = found;
    }
    size_t last_index = get_closest_after(second_index, find_last_numbers(mesh_name));

    if (first_index == std::string::npos || second_index == std::string::npos || last_index == std::string::npos)
    {
        VE_LOG_ERROR("mesh data does not contain a mesh type or a mesh version.");
        throw std::exception();
    }

    mesh::MeshNodeData mesh_name_data = {};
    mesh_name_data.name = mesh_name.substr(0, first_index);
    mesh_name_data.type = get_mesh_type(mesh_name.substr(first_index + 1, second_index - first_index - 1));
    mesh_name_data.version = stoul(mesh_name.substr(second_index + 1, last_index - second_index));
    mesh_name_data.mesh_ptr = ai_mesh;

    return mesh_name_data;
}

const size_t MeshManager::get_closest_after(size_t closest, const std::vector<size_t>& indexes)
{
    size_t smallest_larger_then_closest = std::string::npos;

    for (const auto& index : indexes)
    {
        if ((index > closest) && (index < smallest_larger_then_closest))
        {
            smallest_larger_then_closest = index;
        }
    }

    return smallest_larger_then_closest;
}

const std::vector<size_t> MeshManager::find_last_numbers(const std::string& str)
{
    int    index = -1;
    size_t before_index = 0;

    // Find all integers in string.
    std::vector<bool> is_number;
    is_number.resize(str.size());
    std::fill(is_number.begin(), is_number.end(), false);

    for (int i = 0; i < 10; i++)
    {
        size_t index = str.find(std::to_string(i), 0);
        while (index != std::string::npos)
        {
            is_number.at(index) = true;
            index = str.find(std::to_string(i), index + 1);
        }
    }

    std::vector<size_t> final_nums;
    for (size_t i = 1; i < is_number.size(); i++)
    {
        const auto& is_num = is_number.at(i);

        if (is_num && i + 1 == is_number.size())
        {
            final_nums.push_back(i);
        }

        if (!is_num && is_number.at(i - 1))
        {
            final_nums.push_back(i - 1);
        }
    }

    return final_nums;
}
