#include "MultiIndirectDrawer.h"

using namespace vengine;

MultiIndirectDrawer::MultiIndirectDrawer(MeshManager* mesh_manager, std::vector<mesh::Range>& meshIds)
{
    init(mesh_manager, meshIds);
}

void MultiIndirectDrawer::draw(const unsigned int shader_program) const
{
    glUseProgram(shader_program);
    bind_vao();
    m_mutable_dib.bind();

    glMultiDrawElementsIndirect(GL_TRIANGLES,
                                GL_UNSIGNED_INT, // indices are unsigned ints
                                (GLvoid*)0,      // offset into draw buffer
                                m_num_objects,   // Draw objects
                                0);              // No stride (draw commands are tightly packed)

    // m_mutable_dib.unbind();
    // unbind_vao();
    //  glUseProgram(0);
}

// All init functions
void MultiIndirectDrawer::init(MeshManager* mesh_manager, std::vector<mesh::Range>& meshIds)
{

    // Data from meshManager
    std::vector<float>&        vertexData = mesh_manager->get_vertex_data();
    std::vector<unsigned int>& indexData = mesh_manager->get_index_data();

    // Data to fill
    std::vector<unsigned int>     eboData;
    std::vector<float>            vboData;
    std::vector<IndirectElements> drawIndirectArray;

    MeshManager::MeshVertexType lastMeshType = {};
    for (const mesh::Range& idRange : meshIds)
    {
        for (unsigned int meshId = idRange.first; meshId <= idRange.second; meshId++)
        {

            const mesh::Mesh   mesh = mesh_manager->get_mesh(meshId);
            const mesh::Range& lodMeshIds = mesh.lod_mesh_ids;

            // MeshIds
            this->m_meshIds.push_back(meshId);

            for (unsigned int id = lodMeshIds.first; id <= lodMeshIds.second; id++)
            {
                // Data from MeshManager
                mesh::Range&                 indexRange = mesh_manager->get_index_positions(id);
                mesh::Range&                 vertexRange = mesh_manager->get_vertex_positions(id);
                MeshManager::MeshVertexType& meshType = mesh_manager->get_mesh_vertex_type(id);
                unsigned int                 meshTypeValue = static_cast<unsigned int>(meshType);

                // To ensure lastMeshType actually has a meshType to begin with
                if (m_num_objects == 0)
                {
                    lastMeshType = meshType;
                }

                // Filling vbo and ebo data
                unsigned int firstVertex = vboData.size() / meshTypeValue;
                unsigned int firstIndex = eboData.size();
                vboData.insert(vboData.end(), vertexData.begin() + vertexRange.first, vertexData.begin() + vertexRange.second + 1);
                eboData.insert(eboData.end(), indexData.begin() + indexRange.first, indexData.begin() + indexRange.second + 1);

                // Indirect Array
                IndirectElements elements = {};
                elements.count = indexRange.second - indexRange.first + 1; // vertices to be rendered
                elements.prim_count = 0;                                   // Instances to render
                elements.first_index = firstIndex;                         // position of first index
                elements.base_vertex = firstVertex;                        // position of first vertex
                elements.base_instance = 0;                                // sum of all previous instances
                drawIndirectArray.push_back(elements);

                // Mesh lod id to Dib index
                m_mesh_lodId_to_dib_index.emplace(id, drawIndirectArray.size() - 1);

                // Object
                m_num_objects++;

                // MeshType stuff
                if (lastMeshType != mesh_manager->get_mesh_vertex_type(id))
                {
                    VE_LOG_ERROR("MESH_TYPE IS NOT THE SAME ID : " << id);
                }
                lastMeshType = meshType;
            }
        }
    }

    init_buf_objects(vboData, eboData, drawIndirectArray, lastMeshType);

    vboData.clear();
    eboData.clear();
    drawIndirectArray.clear();
}

void MultiIndirectDrawer::init_buf_objects(std::vector<float>& vboData, std::vector<unsigned int>& eboData,
                                           std::vector<IndirectElements>& drawIndirectArray, MeshManager::MeshVertexType& meshType)
{
    gen_vao();
    bind_vao();

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);


    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    m_mutable_dib.gen(drawIndirectArray.size()); // Auto binds/unbinds

    // copy the data into buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vboData.size(), &vboData.front(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * eboData.size(), &eboData.front(), GL_STATIC_DRAW);
    m_mutable_dib.set_updatebuf_data(&drawIndirectArray.front(), drawIndirectArray.size(), 0);

    MeshManager::set_vertex_attrib_ptrs(meshType);

    unbind_vao();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
