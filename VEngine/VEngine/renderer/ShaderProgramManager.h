/*

        #define SHADER_PRINT_LOGS : to print error logs when validateShader() is called
        #define SHADER_ERROR_DELETE : to delete shader if an error happens
        Must call init() to work.
        Stores programs with the identifier (getProgram) in the form identifier.shader_type.shader
*/

#pragma once

// Dependencies
#include "GL/glew.h"

#include <assert.h>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

// My imports
#define NULL_PROGRAM "NULL"
#define NULL_SHADER  0
#include "../Logger.hpp"
#include "ShaderProgramParser.h"
#include "ShaderUtil.h"

namespace vengine
{

class ShaderProgramManager
{
    public:

        ShaderProgramManager(bool compile_programs, bool use_precompiled_programs, const std::vector<std::string>& shader_program_paths);
        ~ShaderProgramManager() = default;

        void compile();
        void delete_programs();

        // Getters
        int get_program(const char* programName) const
        {
            assert(m_programs.find(programName) != m_programs.end() && "Error: programName isn't a propper shader program");

            return m_programs.at(programName);
        }

        const std::unordered_map<std::string, unsigned int>& get_program_name_to_programId_map() const
        {
            return m_programs;
        }

        const unsigned int get_storage_binding(const std::string& storage_name) const
        {
            const auto& map = m_storage_binding_data.name_to_binding;

            assert(map.find(storage_name) != map.end() && "Error, storage_name not found.");

            return map.at(storage_name);
        }

        const std::unordered_map<std::string, GLenum>& get_storage_name_to_buffer_type() const
        {
            return m_storage_binding_data.name_to_buffer_type;
        }

        const std::unordered_map<std::string, unsigned int>& get_storage_name_to_binding() const
        {
            return m_storage_binding_data.name_to_binding;
        }

        const std::unordered_map<unsigned int, std::vector<std::string>> get_program_to_storage_names() const
        {
            return m_storage_binding_data.program_to_names;
        }

    private:

        struct StorageData
        {
                std::string  storageName = "";
                unsigned int bindingPoint = 0;
                GLenum       storageType = GL_UNIFORM_BUFFER;
        };


        // program data
        std::unordered_map<std::string, unsigned int> m_programs{};
        std::vector<std::string>                      m_shader_paths{};
        bool                                          m_use_precompiled_programs;

        struct StorageBindingData
        {
                std::unordered_map<std::string, GLenum>                    name_to_buffer_type{};
                std::unordered_map<std::string, unsigned int>              name_to_binding{};
                std::unordered_map<unsigned int, std::vector<std::string>> program_to_names{};
        };
        StorageBindingData m_storage_binding_data{};

        // Compiling
        void compile_shader_programs(ShaderProgramParser::ShaderProgramMap& shaderPrograms);
        void create_shaders(std::vector<unsigned int>& compiledShaderIds, std::vector<ShaderProgramParser::Shader>& shaders,
                            std::string programName);
        void create_programs(std::vector<unsigned int>& compiledShaderIds, std::string programName, bool deleteShaders = true);

        // Reflection
        void find_program_storage(const unsigned int& program_id, const std::string& shader_program_name);

        static std::pair<std::string, unsigned int> get_buffer_name_and_binding(const unsigned int& programId,
                                                                                const GLenum&       programInterface1,
                                                                                const GLenum& programInterface2, const unsigned int& index);

        static void set_program_storage_data(const unsigned int program_id, unsigned int i, const std::string& shader_program_name,
                                             StorageBindingData& storage_data, const GLenum& storage_type);

        // Reflection util
        constexpr static auto get_storage_buffer_name(const GLenum& storage_buffer) -> const char*;
        static constexpr auto get_program_interfaces(const GLenum& storage_type) -> std::pair<GLenum, GLenum>;

        // Getters
        inline ShaderProgramParser::ShaderProgramMap get_precompiled_program_map();
};
}; // namespace vengine
