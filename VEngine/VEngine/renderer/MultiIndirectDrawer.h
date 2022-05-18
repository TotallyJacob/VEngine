#pragma once

#include "GL/glew.h"

#include "MeshManager.h"
#include "MutableDib.h"
#include "MutableEbo.h"
#include "MutableVbo.h"
#include "ShaderStorageManager.h"

#include <map>
#include <unordered_map>

#include "../Timer.h"


namespace vengine
{

class MultiIndirectDrawer
{
    public:

        MultiIndirectDrawer(MeshManager* meshManager, std::vector<mesh::Range>& meshIds);

        unsigned int get_num_meshes()
        {
            return m_num_objects;
        }

        unsigned int get_dib_index(unsigned int meshLodId)
        {
            return m_mesh_lodId_to_dib_index.at(meshLodId);
        }

        const std::unordered_map<unsigned int, unsigned int>& get_mesh_lodId_to_dib_index() const
        {
            return m_mesh_lodId_to_dib_index;
        }

        const std::vector<unsigned int>& get_meshIds() const
        {
            return m_meshIds;
        }

        CourierBuffer<IndirectElements>* get_dib_courier()
        {
            return &m_dib_courier;
        }

        MutableDib<3>* get_mutable_dib()
        {
            return &m_mutable_dib;
        }

        void draw(const unsigned int shader_program);

    private:

        std::unordered_map<unsigned int, unsigned int> m_mesh_lodId_to_dib_index{};
        std::vector<unsigned int>                      m_meshIds{};

        unsigned int m_shader_program = 0, m_num_objects = 0;

        // OpenGL buffers
        CourierBuffer<IndirectElements> m_dib_courier = {};
        MutableDib<3>                   m_mutable_dib = {};
        unsigned int                    m_vao = 0, m_vbo = 0, m_ebo = 0;

        // Test things
        void change_instance_count(unsigned int& _numInstances, unsigned int index, const int byHowMuch)
        {
            unsigned int& numInstances = m_mutable_dib.get_persistent_map(index)->prim_count;
            numInstances += byHowMuch;

            // Setting gl_BaseInstance
            for (int i = index + 1; i < m_num_objects; i++)
            {
                m_mutable_dib.get_persistent_map(i)->base_instance += byHowMuch;
            }

            _numInstances = numInstances;
        }
        void change_instance_count(unsigned int index, const int byHowMuch)
        {
            unsigned int& numInstances = m_mutable_dib.get_persistent_map(index)->prim_count;
            numInstances += byHowMuch;

            // Setting gl_BaseInstance
            for (int i = index + 1; i < m_num_objects; i++)
            {
                m_mutable_dib.get_persistent_map(i)->base_instance += byHowMuch;
            }
        }


        void init(MeshManager* mesh_manager, std::vector<mesh::Range>& meshIds);

        void init_buf_objects(std::vector<float>& vboData, std::vector<unsigned int>& eboData,
                              std::vector<IndirectElements>& drawIndirectArray, MeshManager::MeshVertexType& meshType);

        inline void gen_vao()
        {
            glGenVertexArrays(1, &m_vao);
        }

        inline void bind_vao() const
        {
            glBindVertexArray(m_vao);
        }

        inline void unbind_vao() const
        {
            glBindVertexArray(0);
        }
};

}; // namespace vengine
