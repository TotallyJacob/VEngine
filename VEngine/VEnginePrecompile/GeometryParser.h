#pragma once
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

// My imports
#include "Util.hpp"

class GeometryParser
{
    private:

        struct GeometryData
        {
                const char*  name;
                unsigned int fileLength = 0;
                unsigned int mem = 0;
        };
        std::vector<GeometryData> geometryData;

        // Name data
        std::string       geometryPath, outputFile;
        const std::string geometryQualifier = ".obj";

        // Geometry Data
        std::vector<std::filesystem::path> geometryFiles;

        // Writing
        inline static void writeGeometryDataStruct(std::stringstream& ss)
        {
            ss << "struct GeometryData {\n";
            ss << "		const char* name;\n";
            ss << "		unsigned int fileLength = 0;\n";
            ss << "		unsigned int mem = 0;\n";
            ss << "};\n";
        }
        inline static void addGeometryData(std::stringstream& ss, const GeometryData& geometryData)
        {
            ss << "{";
            ss << "\"" + std::string(geometryData.name) + "\"";
            ss << ",";
            ss << geometryData.fileLength;
            ss << ",";
            ss << geometryData.mem;
            ss << "}";
        }
        inline static void addDefaultGeometryData(std::stringstream& ss)
        {
            addGeometryData(ss, {"NULL", 0, 0});
        }

        // Main
        void findGeometry();
        void readGeometry();
        void writeGeometry();

    public:

        GeometryParser(std::string geometryPath, std::string outputFile);

        void parse();
};
