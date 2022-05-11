#pragma once

#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

#include "GL/glew.h"

#include "Util.hpp"

class ShaderProgramParser
{
    private:

        // Data
        using ShaderTag = std::string;
        using Path = std::string;

        struct ShaderData
        {
                std::string shaderType;
                std::string shaderPath;
        };
        struct Shader
        {
                unsigned int shaderType;
                std::string  shaderCode;
        };
        struct ShaderProgram
        {
                std::string  shaderName;
                unsigned int index;
                unsigned int numberOfShaders;
        };

        static constexpr unsigned int                  shaderTagSize = 5;
        unsigned int                                   numberOfShaders = 0, numberOfShaderPrograms = 0;
        std::map<std::string, std::vector<ShaderData>> shaderProgramPaths;
        std::vector<Shader>                            shaders;
        std::vector<ShaderProgram>                     shaderPrograms;

        // Shader tags
        inline static bool insertShaderBefore(std::string& newShaderTag, std::string& addedShaderTag)
        {
            return shaderTypeValue(newShaderTag) < shaderTypeValue(addedShaderTag);
        }
        inline static unsigned int shaderTypeValue(std::string& type)
        {
            if (type == "vert")
                return 1;
            if (type == "geom")
                return 2;
            if (type == "frag")
                return 3;
        }
        inline static const unsigned int getShaderType(std::string type)
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

        // Writing Data
        inline static void writeDefaultShader(std::stringstream& output, size_t num_shaders)
        {
            output << "   {0,\"NULL\"}";

            if (num_shaders != 0)
            {
                output << ",";
            }

            util::newLine(output);
        }
        inline static void writeDefaultProgram(std::stringstream& output, size_t num_programs)
        {
            output << "   { \"NULL\",0,0}";

            if (num_programs != 0)
            {
                output << ",";
            }
            util::newLine(output);
        }
        inline static void writeShaderProgramStruct(std::stringstream& ss)
        {
            ss << "struct ShaderProgram { \n";
            ss << "const char* programName;\n";
            ss << "unsigned int index;\n";
            ss << "unsigned int numberOfShaders;\n";
            ss << "};\n";
        }
        inline static void writeShaderStruct(std::stringstream& ss)
        {
            ss << "struct Shader { \n";
            ss << "unsigned int type;\n";
            ss << "const char* code;\n";
            ss << "};\n";
        }

        void findShaders(const char* path);
        void readShaders();

    public:

        ShaderProgramParser(const char* path);
        ~ShaderProgramParser() = default;

        void write(const char* path);
};
