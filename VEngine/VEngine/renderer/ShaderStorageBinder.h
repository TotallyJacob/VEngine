#pragma once

#include "GL/glew.h"

#include "../Logger.hpp"
#include "IMutableBuffer.h"
#include "ShaderProgramManager.h"

#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace vengine
{

class ShaderStorageBinder
{
    public:

        ShaderStorageBinder(const unsigned int max_ssbo_bindings, const unsigned int max_ubo_bindings,
                            const std::unordered_map<unsigned int, unsigned int> storageId_to_bufferId,
                            const std::unordered_map<std::string, unsigned int>  name_to_storageId,
                            std::vector<std::unique_ptr<IMutableBuffer>>& mutable_storages, ShaderProgramManager& shader_program_manager);
        ~ShaderStorageBinder() = default;

        void add_shader_storage_binding_to_program(const unsigned int& storageId, const unsigned int& programId);
        void add_shader_storage(const unsigned int& storageId, const GLenum& target, const GLint& index, const GLint& buffer,
                                IMutableBuffer& mutable_storage);

        void               bind_storages(const unsigned int& program);
        const unsigned int get_binding_point_offset(const GLenum& target) const;

    private:

        std::vector<std::unique_ptr<IMutableBuffer>>* m_shader_storage_interfaces;

        struct BindBufferRangeData
        {
                GLenum target = 0; // we should know target ahead of time
                GLuint index = 0;
                GLuint buffer = 0;
        };
        std::vector<BindBufferRangeData> m_bbr_data{};
        std::vector<unsigned int*>       m_bbr_changed_at_index{};

        std::unordered_map<unsigned int, unsigned int>              m_storageId_to_bbr_index{};
        std::unordered_map<unsigned int, std::vector<unsigned int>> m_programId_to_storageId{};

        std::vector<unsigned int> m_binding_point_to_bbr_bound{};
        unsigned int              m_max_ssbo_bindings = 0;
        unsigned int              m_max_ubo_bindings = 0;


        void               check_bindings_are_unique(const std::unordered_map<std::string, unsigned int> name_to_storageId) const;
        const unsigned int remove_binding_point_offset(const unsigned int binding_point) const;
        void               assert_buffer_binding_index(const GLenum& target, const GLuint& index) const;
        void               set_bind_buffer_range_data(const GLenum& target, const GLuint& index, const GLuint& buffer);
        void               init_binding_to_bbr_bound(const unsigned int max_ssbo_bindings, const unsigned int max_ubo_bindings);
};
}; // namespace vengine
