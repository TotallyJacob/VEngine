#pragma once
#include "../FileUtil.hpp"
#include "GL/glew.h"


#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

namespace vengine
{

class ShaderProgramParser
{
    public:

        struct Shader
        {
                unsigned int shaderType;
                std::string  shaderCode;
        };

        typedef std::map<std::string, std::vector<ShaderProgramParser::Shader>> ShaderProgramMap;


        ShaderProgramParser(const std::vector<std::string>& paths, bool findReadShaderPrograms);
        ~ShaderProgramParser() = default;

        void find_read_shader_programs();

        // Assume it's correct
        inline auto get_shaders(std::string shaderTag) -> std::vector<Shader>&
        {
            return shaderPrograms.at(shaderTag);
        }
        inline auto get_shader_programs(std::string shaderTag) -> std::pair<std::string, std::vector<Shader>&>
        {
            return std::pair<std::string, std::vector<Shader>&>(shaderTag, shaderPrograms.at(shaderTag));
        }
        inline auto get_shader_programs() -> ShaderProgramMap&
        {
            return shaderPrograms;
        }

        inline void clear_shader_program_data()
        {
            shaderPrograms.clear();
        }

    private:

        static constexpr unsigned int shaderTagSize = 5;
        ShaderProgramMap              shaderPrograms;
        std::vector<std::string>      m_paths{};

        inline static const unsigned int get_shader_type(std::string type)
        {
            if (type == "frag")
                return GL_FRAGMENT_SHADER;

            if (type == "vert")
                return GL_VERTEX_SHADER;

            if (type == "geom")
                return GL_GEOMETRY_SHADER;

            if (type == "comp")
                return GL_COMPUTE_SHADER;

            if (type == "ctrl")
                return GL_TESS_CONTROL_SHADER;

            if (type == "eval")
                return GL_TESS_EVALUATION_SHADER;
        }
        inline static const unsigned int get_shader_value(unsigned int shaderType)
        {
            switch (shaderType)
            {
                case GL_VERTEX_SHADER:
                    return 1;
                case GL_GEOMETRY_SHADER:
                    return 2;
                case GL_FRAGMENT_SHADER:
                    return 3;
                default:
                    return 5;
            }
        }
        inline static const bool insert_before(unsigned int shaderToInsertBefore, unsigned int shader)
        {
            return get_shader_value(shaderToInsertBefore) < get_shader_value(shader);
        }
        inline const std::string read_shader(std::string path) const
        {
            unsigned int lengthOfData = 0;
            std::string  shaderCode;


            readData(path, lengthOfData, shaderCode);

            return shaderCode;
        }

        void order_programs();
};
}; // namespace vengine
