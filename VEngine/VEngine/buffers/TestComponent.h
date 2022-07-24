#pragma once

#include "GL/glew.h"

#include "../ByteArray.h"

namespace buf
{
// BC

struct TestBC
{

        GLuint     buffer = 0;
        GLsizeiptr bytes = 0;
        char*      persistent_map = nullptr;
};

struct FlagsBC
{
        GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT;
};


struct ElementsBC
{
        unsigned int num_elements = 0;
};

// PC

class TestPC
{
        char* persistent_map = nullptr;
};

}; // namespace buf