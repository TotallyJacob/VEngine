#include "GeometryParser.h"

GeometryParser::GeometryParser(std::string geometryPath, std::string outputFile) : geometryFiles(0), geometryData(0)
{
    this->geometryPath = geometryPath;
    this->outputFile = outputFile;
}

// Public
void GeometryParser::parse()
{
    findGeometry();
    readGeometry();
    writeGeometry();
}

// Private
void GeometryParser::findGeometry()
{
    util::getFiles(geometryPath, geometryFiles, geometryQualifier);
}
void GeometryParser::readGeometry()
{
    for (const auto filePath : geometryFiles)
    {

        unsigned int length = 0;

        char* data = util::readData(filePath, length);

        delete[] data;

        std::string              stem = filePath.stem().string();
        std::vector<std::string> splitStem = util::split(stem, '.');

        geometryData.push_back({splitStem[0].c_str(), length, 0});
    }
}
void GeometryParser::writeGeometry()
{
    std::stringstream out;

    writeGeometryDataStruct(out);

    util::newLine(out);

    // Writing geometryData
    out << "constexpr unsigned int geometryDataLength = ";
    out << geometryData.size() + 1;
    out << ";\n";
    out << "GeometryData geometryData[geometryDataLength] = {\n";
    addDefaultGeometryData(out);
    for (int i = 0; i < geometryData.size(); i++)
    {
        const GeometryData& data = geometryData.at(i);

        addGeometryData(out, data);

        if (i != geometryData.size() - 1)
            out << ",";

        out << "\n";
    }
    out << "};\n";

    util::newLine(out);

    // Writing
    std::string data = out.str();
    util::writeData(outputFile, data.data(), data.size());
}
