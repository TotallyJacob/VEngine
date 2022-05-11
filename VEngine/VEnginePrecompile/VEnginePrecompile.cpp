#include <fstream>
#include <iostream>

// My imports
#include "GeometryParser.h"
#include "ShaderProgramParser.h"


int main()
{

    /*SHADER*/

    ShaderProgramParser parser(util::readPath);
    parser.write(std::string(std::string(util::writePath) + "\\Shaders.hpp").c_str());

    /*GEOMETRY*/

    // GeometryParser p(util::readPath, std::string(std::string(util::writePath) + "\\Geometry.hpp").c_str());
    // p.parse();

    return 0;
}
