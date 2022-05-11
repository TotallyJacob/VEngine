#include "ShaderProgramParser.h"

ShaderProgramParser::ShaderProgramParser(const char* path)
{

    findShaders(path);
    readShaders();
}

// Private
void ShaderProgramParser::findShaders(const char* path)
{

    namespace fs = std::filesystem;

    std::vector<fs::path> paths;
    util::getFiles(path, paths, ".shader");

    numberOfShaders = paths.size();

    std::string pathName, shaderTag;
    for (auto path : paths)
    {

        pathName = path.stem().string();
        shaderTag = pathName.substr(0, pathName.size() - shaderTagSize);

        ShaderData data = {};
        data.shaderPath = path.string();
        data.shaderType = pathName.substr(shaderTag.size() + 1, 4);


        if (shaderProgramPaths.find(shaderTag) != shaderProgramPaths.end())
        {

            unsigned int             insertIndex = 0;
            std::vector<ShaderData>& shaderDatas = shaderProgramPaths.at(shaderTag);
            for (auto& shaderData : shaderDatas)
            {

                if (insertShaderBefore(data.shaderType, shaderData.shaderType))
                {
                    shaderDatas.insert(shaderDatas.begin() + insertIndex, data);
                    break;
                }

                insertIndex++;
            }

            if (insertIndex != shaderDatas.size())
                continue;

            shaderProgramPaths.at(shaderTag).push_back(data);
            continue;
        }

        shaderProgramPaths.emplace(shaderTag, std::vector<ShaderData>(0));
        shaderProgramPaths.at(shaderTag).push_back(data);
    }
}
void ShaderProgramParser::readShaders()
{

    unsigned int index = 0, numberOfShaders = 0, lengthOfData = 0;
    for (auto& program : shaderProgramPaths)
    {

        numberOfShaders = 0, lengthOfData = 0;
        // Reading data
        for (auto& shaderData : program.second)
        {
            char* data = util::readData(shaderData.shaderPath, lengthOfData);

            shaders.push_back({getShaderType(shaderData.shaderType), std::string(data)});
            numberOfShaders++;

            delete[] data;
        }

        shaderPrograms.push_back({program.first, index, numberOfShaders});

        index += numberOfShaders;
    }
}

// Public
void ShaderProgramParser::write(const char* path)
{
    std::stringstream output;

    writeShaderStruct(output);
    writeShaderProgramStruct(output);

    util::newLine(output);

    output << "constexpr unsigned int numberOfShaderPrograms = 1 + ";
    output << shaderPrograms.size();
    output << ";\n";

    output << "constexpr unsigned int numberOfShaders = 1 + ";
    output << shaders.size();
    output << ";\n";

    util::newLine(output);

    // Writing shaderPrograms
    output << " ShaderProgram programs[numberOfShaderPrograms] = {\n";
    writeDefaultProgram(output, shaderPrograms.size());
    for (int i = 0; i < shaderPrograms.size(); i++)
    {
        ShaderProgram& program = shaderPrograms.at(i);

        output << "   {";
        output << "\"" + program.shaderName + "\"";
        output << ",";
        output << program.index;
        output << ",";
        output << program.numberOfShaders;
        output << "}";

        if (i != shaderPrograms.size() - 1)
            output << ",";

        util::newLine(output);
    }
    output << "};\n";

    util::newLine(output);

    // Writing Shaders
    output << "Shader shaders[numberOfShaders] = {\n";
    writeDefaultShader(output, shaders.size());
    for (int i = 0; i < shaders.size(); i++)
    {
        Shader& shader = shaders.at(i);

        output << "   {";
        output << shader.shaderType;
        output << ",";
        output << "\"";
        for (char c : shader.shaderCode)
        {

            if (c == '\r')
            {
                output << "\\r";
                continue;
            }

            if (c == '\n')
            {
                output << "\\n";
                continue;
            }

            output << c;
        }
        output << "\"";
        output << "}";

        if (i != shaders.size() - 1)
            output << ",";

        util::newLine(output);
    }
    output << "};\n";

    // Writing back to file
    std::string data = output.str();
    util::writeData(path, data.data(), data.size());
}
