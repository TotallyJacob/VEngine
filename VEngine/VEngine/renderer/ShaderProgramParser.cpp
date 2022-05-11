#include "ShaderProgramParser.h"

using namespace vengine;

ShaderProgramParser::ShaderProgramParser(const std::vector<std::string>& paths, bool findReadPrograms)
{
    this->m_paths = paths;
    if (findReadPrograms)
    {
        find_read_shader_programs();
    }
}

void ShaderProgramParser::find_read_shader_programs()
{
    namespace fs = std::filesystem;

    for (auto& path : m_paths)
    {

        std::vector<fs::path> paths;
        getFiles(path, paths, ".shader");

        std::string pathName, shaderTag;
        for (auto path : paths)
        {

            pathName = path.stem().string();
            shaderTag = pathName.substr(0, pathName.size() - shaderTagSize);

            Shader shader = {};
            shader.shaderType = get_shader_type(pathName.substr(shaderTag.size() + 1, 4));
            shader.shaderCode = read_shader(path.string());

            if (shaderPrograms.find(shaderTag) != shaderPrograms.end())
            {
                shaderPrograms.at(shaderTag).push_back(shader);
                continue;
            }

            shaderPrograms.emplace(shaderTag, std::vector<Shader>(0));
            shaderPrograms.at(shaderTag).push_back(shader);
        }
    }


    // orderPrograms(); //Annoying story of forgetting continue;
}

void ShaderProgramParser::order_programs()
{

    unsigned int        insertPosition = 0;
    std::vector<Shader> newShaderOrdering;

    for (auto& shaderProgram : shaderPrograms)
    {

        for (auto shader : shaderProgram.second)
        {

            if (newShaderOrdering.size() == 0)
            {
                newShaderOrdering.push_back(shader);
                continue;
            }


            insertPosition = 0;

            // Loop over all shaders in the newShaderOrdering
            for (auto shaders : newShaderOrdering)
            {
                if (insert_before(shader.shaderType, shaders.shaderType))
                {
                    newShaderOrdering.insert(newShaderOrdering.begin() + insertPosition, shader);
                    break;
                }
                insertPosition++;
            }
        }

        shaderProgram.second = newShaderOrdering;
        newShaderOrdering.clear();
    }
}
