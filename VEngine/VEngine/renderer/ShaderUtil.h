/*

        #define SHADER_PRINT_LOGS : to print error logs when validateShader() is called
        #define SHADER_ERROR_DELETE : to delete shader if an error happens

*/

#pragma once

// Dependencies
#include "GL/glew.h"

#include "../Logger.hpp"

// My defines
#define SHADER_PRINT_LOGS
#define SHADER_ERROR_DELETE
#define SHADER_LOG_SIZE 512

namespace vengine::util
{

// Shaders
inline static void create_shader(unsigned int& shaderId, unsigned int& shaderType, const char* source)
{
    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &source, 0);
    glCompileShader(shaderId);
}
inline static void validate_shader(unsigned int& shader, int success, std::string name)
{
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {

        char infoLog[SHADER_LOG_SIZE];
        glGetShaderInfoLog(shader, SHADER_LOG_SIZE, NULL, infoLog);

#ifdef SHADER_PRINT_LOGS
        VE_LOG_ERROR("Shader: " << name << " " << shader << "\n" << infoLog);
#endif

#ifdef SHADER_ERROR_DELETE
        glDeleteShader(shader);
#endif
    }
}

// Shader programs
inline static void validate_shader_program(unsigned int& shaderProgram, int success, std::string name)
{
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // Print errors
    if (success == GL_FALSE)
    {
        char infoLog[SHADER_LOG_SIZE];
        glGetProgramInfoLog(shaderProgram, GL_LINK_STATUS, NULL, infoLog);

#ifdef SHADER_PRINT_LOGS
        VE_LOG_ERROR("Program: " << name << " " << shaderProgram << "\n" << infoLog);
#endif

#ifdef SHADER_ERROR_DELETE
        glDeleteProgram(shaderProgram);
#endif
    }
}
}; // namespace vengine::util
