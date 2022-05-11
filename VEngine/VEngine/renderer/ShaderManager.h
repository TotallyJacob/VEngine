#pragma once

#include "ShaderProgramManager.h"
#include "ShaderStorageManager.h"

namespace vengine
{

class ShaderManager
{
    public:

        ShaderManager(bool compilePrograms, bool usePrecompilePrograms = true,
                      const std::vector<std::string>& shaderProgramPath = {"NULL"});
        ~ShaderManager() = default;

        ShaderStorageManager& get_storage_manager()
        {
            return m_storage_manager;
        }

        ShaderProgramManager& get_program_manager()
        {
            return m_program_manager;
        }

    private:

        ShaderStorageManager m_storage_manager;
        ShaderProgramManager m_program_manager;
};
} // namespace vengine
