#include "ShaderStorageBinder.h"

using namespace vengine;

ShaderStorageBinder::ShaderStorageBinder(const unsigned int max_ssbo_bindings, const unsigned int max_ubo_bindings,
                                         const std::unordered_map<unsigned int, unsigned int> storageId_to_bufferId,
                                         const std::unordered_map<std::string, unsigned int>  name_to_storageId,
                                         std::vector<std::unique_ptr<IMutableShaderStorage>>& mutable_storages,
                                         ShaderProgramManager&                                shader_program_manager)
{

    init_binding_to_bbr_bound(max_ssbo_bindings, max_ubo_bindings);

    m_shader_storage_interfaces = &mutable_storages;

    for (const auto& name_and_storageId : name_to_storageId)
    {

        const auto& name = name_and_storageId.first;
        const auto& storageId = name_and_storageId.second;

        const auto& buffer = storageId_to_bufferId.at(storageId);

        const auto& name_to_buffer_type = shader_program_manager.get_storage_name_to_binding();
        if (name_to_buffer_type.find(name) == name_to_buffer_type.end())
        {
            VE_LOG_ERROR("Error cannot find shader storage named \`" << name << "\'");
            throw std::exception();
        }

        const auto& target = shader_program_manager.get_storage_name_to_buffer_type().at(name);
        const auto& index = shader_program_manager.get_storage_binding(name);
        auto&       mutable_storage = mutable_storages.at(storageId);


        add_shader_storage(storageId, target, index, buffer, *mutable_storage);

        VE_LOG_DEBUG("name \'" << name << "\' has bbr index " << m_storageId_to_bbr_index.at(storageId));
    }

    for (const auto& program_to_names : shader_program_manager.get_program_to_storage_names())
    {
        const auto& program = program_to_names.first;
        const auto& storage_names = program_to_names.second;
        for (const auto& storage_name : storage_names)
        {
            if (name_to_storageId.find(storage_name) == name_to_storageId.end())
            {
                VE_LOG_WARNING("Warning, not binding by ShaderStorageBinder with storage called \`" << storage_name << "\`");
                continue;
            }
            const auto& storageId = name_to_storageId.at(storage_name);

            add_shader_storage_binding_to_program(storageId, program);
        }
    }

    VE_DEBUG_DO(check_bindings_are_unique(name_to_storageId));
}


void ShaderStorageBinder::add_shader_storage_binding_to_program(const unsigned int& storageId, const unsigned int& programId)
{
    assert(m_storageId_to_bbr_index.find(storageId) != m_storageId_to_bbr_index.end() && "Error, could not find storageId.");

    const auto& bbr_index = m_storageId_to_bbr_index.at(storageId);
    auto&       map = m_programId_to_storageId;

    if (map.find(programId) == map.end())
    {
        std::vector<unsigned int> temp_vec;
        temp_vec.push_back(storageId);
        map.emplace(programId, temp_vec);
    }
    else
    {
        map.at(programId).push_back(storageId);
    }
}
void ShaderStorageBinder::add_shader_storage(const unsigned int& storageId, const GLenum& target, const GLint& index, const GLint& buffer,
                                             IMutableShaderStorage& mutable_storage)
{
    m_bbr_changed_at_index.push_back(&mutable_storage.binding_data_changed());
    m_storageId_to_bbr_index.emplace(storageId, m_bbr_data.size());
    set_bind_buffer_range_data(target, index, buffer);
}

void ShaderStorageBinder::bind_storages(const unsigned int& program)
{
    for (const auto& storageId : m_programId_to_storageId.at(program))
    {
        const auto&  bbr_index = m_storageId_to_bbr_index.at(storageId);
        const auto&  bbr_data = m_bbr_data.at(bbr_index);
        unsigned int bbr_data_changed = *m_bbr_changed_at_index.at(bbr_index);
        auto&        binding_point_to_bbr = m_binding_point_to_bbr_bound.at(bbr_data.index + get_binding_point_offset(bbr_data.target));

        if (bbr_data_changed == true || binding_point_to_bbr != bbr_index)
        {
            m_shader_storage_interfaces->at(storageId)->bind_readbuf_range(bbr_data.target, bbr_data.index, bbr_data.buffer);

            binding_point_to_bbr = bbr_index;
            bbr_data_changed = false;
        }
    }
}

const unsigned int ShaderStorageBinder::get_binding_point_offset(const GLenum& target) const
{
    assert(target == GL_SHADER_STORAGE_BUFFER || target == GL_UNIFORM_BUFFER && "Error, invalid target.");

    switch (target)
    {
        case GL_SHADER_STORAGE_BUFFER:
            return 0;

        case GL_UNIFORM_BUFFER:
            return 0 + m_max_ssbo_bindings;
    }
}



void ShaderStorageBinder::check_bindings_are_unique(const std::unordered_map<std::string, unsigned int> name_to_storageId) const
{

    // For suggestions
    std::map<GLenum, std::vector<unsigned int>> unused_binding_points{};
    unused_binding_points.emplace(GL_SHADER_STORAGE_BUFFER, std::vector<unsigned int>(0));
    unused_binding_points.emplace(GL_UNIFORM_BUFFER, std::vector<unsigned int>(0));

    // Pushing back binding info.
    {
        auto& ssbo_bps = unused_binding_points.at(GL_SHADER_STORAGE_BUFFER);
        for (int i = 0; i < m_max_ssbo_bindings; i++)
        {
            ssbo_bps.push_back(i);
        }

        auto& ubo_bps = unused_binding_points.at(GL_UNIFORM_BUFFER);
        for (int i = 0; i < m_max_ubo_bindings; i++)
        {
            ubo_bps.push_back(i);
        }
    }


    // For finding conflicts
    std::vector<std::tuple<std::string, GLenum, unsigned int>> storage_with_binding_conflict{};
    std::map<unsigned int, std::string>                        binding_point_to_storage{};
    for (const auto& pair : name_to_storageId)
    {
        // Pair data
        const auto& storage_name = pair.first;
        const auto& storageId = pair.second;

        // bbr data
        const auto& bbr_index = m_storageId_to_bbr_index.at(storageId);
        const auto& bbr_data = m_bbr_data.at(m_storageId_to_bbr_index.at(storageId));
        const auto& target = bbr_data.target;
        const auto& binding_point = bbr_data.index;

        if (binding_point_to_storage.find(binding_point) == binding_point_to_storage.end())
        {
            binding_point_to_storage.emplace(binding_point, storage_name);

            // Remove used binding points
            auto& bp_to_use = unused_binding_points.at(target);
            std::remove(bp_to_use.begin(), bp_to_use.end(), binding_point);
        }
        else
        {
            storage_with_binding_conflict.push_back(std::make_tuple(storage_name, target, binding_point));

            VE_LOG_WARNING_MULTI_START("Warning, conflicting shader storage binding point " << binding_point);
            VE_LOG_WARNING_MULTI("storage named \'" << storage_name << "\' is bound to the same binding point as storage as");
            VE_LOG_WARNING_MULTI("\'" << binding_point_to_storage.at(binding_point) << "\'");
            VE_LOG_WARNING_MULTI_END();
        }
    }

    // Suggestions
    for (const auto& tuple : storage_with_binding_conflict)
    {
        const auto& name = std::get<0>(tuple);
        const auto& type = std::get<1>(tuple);
        const auto& bp = std::get<2>(tuple);

        auto&              bp_arr = unused_binding_points.at(type);
        const unsigned int bp_could_use = bp_arr.at(0);

        VE_LOG_SUGGESTION("could bind storage named \'" << name << "\' to binding point " << bp_could_use << " instead.");

        bp_arr.erase(bp_arr.begin());
    }
}

const unsigned int ShaderStorageBinder::remove_binding_point_offset(const unsigned int binding_point) const
{
    return (binding_point >= m_max_ssbo_bindings) ? binding_point - m_max_ssbo_bindings : binding_point;
}


void ShaderStorageBinder::assert_buffer_binding_index(const GLenum& target, const GLuint& index) const
{
    switch (target)
    {
        case GL_SHADER_STORAGE_BUFFER:
            assert(m_max_ssbo_bindings > index && "Error, binding point index is too large");
            break;
        case GL_UNIFORM_BUFFER:
            assert(m_max_ubo_bindings > index && "Error, binding point index is too large");
            break;
    }
}

void ShaderStorageBinder::set_bind_buffer_range_data(const GLenum& target, const GLuint& index, const GLuint& buffer)
{
    assert_buffer_binding_index(target, index);

    BindBufferRangeData data = {};
    data.target = target;
    data.index = index;
    data.buffer = buffer;

    m_bbr_data.push_back(data);
}

void ShaderStorageBinder::init_binding_to_bbr_bound(const unsigned int max_ssbo_bindings, const unsigned int max_ubo_bindings)
{
    m_max_ssbo_bindings = max_ssbo_bindings;
    m_max_ubo_bindings = max_ubo_bindings;

    m_binding_point_to_bbr_bound.resize(max_ssbo_bindings + max_ubo_bindings);
}
