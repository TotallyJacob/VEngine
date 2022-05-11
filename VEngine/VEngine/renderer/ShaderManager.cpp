#include "ShaderManager.h"

using namespace vengine;

ShaderManager::ShaderManager(bool compilePrograms, bool usePrecompiledPrograms, const std::vector<std::string>& shaderProgramPath)
    : m_storage_manager(), m_program_manager(compilePrograms, usePrecompiledPrograms, shaderProgramPath)
{
}
