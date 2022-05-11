#include "ShaderProgramManager.h"

using namespace vengine;


ShaderProgramManager::ShaderProgramManager(bool compilePrograms, bool usePrecompiledPrograms,
                                           const std::vector<std::string>& shaderProgramPaths)
{
    this->m_use_precompiled_programs = usePrecompiledPrograms;
    m_shader_paths = shaderProgramPaths;

    if (compilePrograms)
    {
        this->compile();
    }
}


void ShaderProgramManager::compile()
{

    ShaderProgramParser::ShaderProgramMap data;

    if (m_use_precompiled_programs)
    {
        data = get_precompiled_program_map();
        compile_shader_programs(data);

        return;
    }

    ShaderProgramParser parser(m_shader_paths, true);
    compile_shader_programs(parser.get_shader_programs());
    parser.clear_shader_program_data();
}

void ShaderProgramManager::delete_programs()
{
    for (auto& program : m_programs)
    {
        glDeleteProgram(program.second);
    }
}

ShaderProgramParser::ShaderProgramMap ShaderProgramManager::get_precompiled_program_map()
{

#include "Shaders.hpp"

    ShaderProgramParser::ShaderProgramMap    data;
    std::vector<ShaderProgramParser::Shader> shaderProgramShaders;

    // Loop over every shaderPrograms
    for (int i = 1; i < numberOfShaderPrograms; i++)
    {

        ShaderProgram& shaderProgram = programs[i];

        unsigned int true_index = shaderProgram.index + 1;
        unsigned int numShadersInProgram = shaderProgram.numberOfShaders;

        // Find all shaders in a shader program
        for (int i = true_index; i < numShadersInProgram + true_index; i++)
        {
            Shader& shader = shaders[i];

            ShaderProgramParser::Shader shaderData = {};
            shaderData.shaderType = shader.type;
            shaderData.shaderCode = shader.code;

            shaderProgramShaders.push_back(shaderData);
        }

        data.emplace(shaderProgram.programName, shaderProgramShaders);
        shaderProgramShaders.clear();
    }

    return data;
}

// Actual shader Compilation code
void ShaderProgramManager::compile_shader_programs(ShaderProgramParser::ShaderProgramMap& shaderPrograms)
{
    std::vector<unsigned int> compiledShaderIds;
    for (auto program : shaderPrograms)
    {

        auto tag = program.first;
        auto shaders = program.second;

        create_shaders(compiledShaderIds, shaders, tag);
        create_programs(compiledShaderIds, tag, true);

        compiledShaderIds.clear();
    }
}

void ShaderProgramManager::create_shaders(std::vector<unsigned int>& compiledShaderIds, std::vector<ShaderProgramParser::Shader>& shaders,
                                          std::string programName)
{

    unsigned int shaderId = 0, success = 0;


    for (auto& shader : shaders)
    {

        shaderId = 0;

        util::create_shader(shaderId, shader.shaderType, shader.shaderCode.c_str());
        util::validate_shader(shaderId, success, programName);

        compiledShaderIds.push_back(shaderId);
    }
}

void ShaderProgramManager::create_programs(std::vector<unsigned int>& compiledShaderIds, std::string programName, bool deleteShaders)
{

    unsigned int shaderProgramId = 0, success = 0;

    shaderProgramId = glCreateProgram();

    for (auto shaderId : compiledShaderIds)
    {
        glAttachShader(shaderProgramId, shaderId);
    }

    glLinkProgram(shaderProgramId);

    util::validate_shader_program(shaderProgramId, success, programName);

    this->m_programs.emplace(programName, shaderProgramId);

    // Delete shaders
    if (deleteShaders)
    {
        for (unsigned int compiledShader : compiledShaderIds)
        {
            glDeleteShader(compiledShader);
        }
    }

    find_program_storage(shaderProgramId, programName);
}


// Reflection
void ShaderProgramManager::find_program_storage(const unsigned int& program_id, const std::string& shader_program_name)
{
    GLint numBlocks[1] = {0};

    // ubo
    glGetProgramInterfaceiv(program_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks[0]);
    for (int i = 0; i < numBlocks[0]; i++)
    {
        set_program_storage_data(program_id, i, shader_program_name, m_storage_binding_data, GL_UNIFORM_BUFFER);
    }

    // ssbo
    glGetProgramInterfaceiv(program_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks[0]);
    for (int i = 0; i < numBlocks[0]; i++)
    {
        set_program_storage_data(program_id, i, shader_program_name, m_storage_binding_data, GL_SHADER_STORAGE_BUFFER);
    }
}

void ShaderProgramManager::set_program_storage_data(const unsigned int program_id, unsigned int i, const std::string& shader_program_name,
                                                    StorageBindingData& storage_data, const GLenum& storage_type)
{
    const auto& interfaces = get_program_interfaces(storage_type);
    const auto& name_binding = get_buffer_name_and_binding(program_id, interfaces.first, interfaces.second, i);

    // Setting buffer type info
    auto& name_to_buffer_type = storage_data.name_to_buffer_type;
    if (name_to_buffer_type.find(name_binding.first) != name_to_buffer_type.end())
    {
        VE_DEBUG_DO(

            const auto& buffer_type = name_to_buffer_type.at(name_binding.first);

            if (buffer_type != storage_type) {
                VE_LOG_ERROR_MULTI_START("Must use a unique name for each shader storage reguardless of type.");
                VE_LOG_ERROR_MULTI("shader storage " << name_binding.first << " is defined as a " << get_storage_buffer_name(storage_type));
                VE_LOG_ERROR_MULTI("in program " << shader_program_name << " . While being defined as a ");
                VE_LOG_ERROR_MULTI(get_storage_buffer_name(buffer_type) << " elsewhere.");
                VE_LOG_ERROR_MULTI_END();

                assert(false && "same shader storage name, but with different buffer_type. See VE_LOG_ERROR for more info.");
            });
    }
    else
    {
        name_to_buffer_type.emplace(name_binding.first, storage_type);
    }

    // Setting binding info
    auto& name_to_binding = storage_data.name_to_binding;
    if (name_to_binding.find(name_binding.first) != name_to_binding.end())
    {
        VE_DEBUG_DO(

            const auto& binding_point = name_to_binding.at(name_binding.first);

            if (binding_point != name_binding.second) {
                VE_LOG_ERROR_MULTI_START("Must define a single " << get_storage_buffer_name(storage_type)
                                                                 << " with a single binding point.");
                VE_LOG_ERROR_MULTI(get_storage_buffer_name(storage_type) << " named " << name_binding.first << " already found.");
                VE_LOG_ERROR_MULTI(shader_program_name << " specifies binding point: " << name_binding.second);
                VE_LOG_ERROR_MULTI("when binding point is already defined as: " << binding_point);
                VE_LOG_ERROR_MULTI_END();

                assert(false && "same shader storage name, but with differn't binding point. See VE_LOG_ERROR for more info.");
            }

        );
    }
    else
    {
        name_to_binding.emplace(name_binding.first, name_binding.second);
    }

    // Setting program to names info
    auto& program_to_names = storage_data.program_to_names;
    if (program_to_names.find(program_id) != program_to_names.end())
    {
        auto& name_vec = program_to_names.at(program_id);
        name_vec.push_back(name_binding.first);
    }
    else
    {
        std::vector<std::string> temp_vec;
        temp_vec.push_back(name_binding.first);
        program_to_names.emplace(program_id, temp_vec);
    }
}

std::pair<std::string, unsigned int> ShaderProgramManager::get_buffer_name_and_binding(const unsigned int& programId,
                                                                                       const GLenum&       programInterface1,
                                                                                       const GLenum&       programInterface2,
                                                                                       const unsigned int& index)
{
    std::pair<std::string, unsigned int> pair = {};

    GLint   query_int[1] = {0};
    GLsizei size_int[1] = {0};
    GLenum  parameters[1] = {GL_NAME_LENGTH};

    // Name
    glGetProgramResourceiv(programId, programInterface2, index, 1, &parameters[0], 1, NULL, &query_int[0]);

    std::string& name = pair.first;
    name.resize(query_int[0]);
    glGetProgramResourceName(programId, programInterface1, index, query_int[0], &size_int[0], &name[0]);
    name.resize(size_int[0]);


    // Buffer binding point
    parameters[0] = {GL_BUFFER_BINDING};
    glGetProgramResourceiv(programId, programInterface1, index, 1, &parameters[0], 1, NULL, &query_int[0]);
    pair.second = query_int[0];

    return pair;
}


// Util functions
constexpr const char* ShaderProgramManager::get_storage_buffer_name(const GLenum& storage_buffer)
{
    switch (storage_buffer)
    {
        case GL_SHADER_STORAGE_BUFFER:
            return " ssbo ";

        case GL_UNIFORM_BUFFER:
            return " ubo ";

        default:
            return " undefined type";
    };
}

constexpr std::pair<GLenum, GLenum> ShaderProgramManager::get_program_interfaces(const GLenum& storage_type)
{
    using ReturnPair = std::pair<GLenum, GLenum>;

    switch (storage_type)
    {
        case GL_SHADER_STORAGE_BUFFER:
            return ReturnPair(GL_SHADER_STORAGE_BLOCK, GL_BUFFER_VARIABLE);

        case GL_UNIFORM_BUFFER:
            return ReturnPair(GL_UNIFORM_BLOCK, GL_UNIFORM);

        default:
            VE_LOG_ERROR("Error, storage_type is invalid, thus, cannot get_program_interfaces.");
            assert(false && "storage_type is invalid.");
            return ReturnPair(GL_SHADER_STORAGE_BLOCK, GL_BUFFER_VARIABLE);
    }
}
