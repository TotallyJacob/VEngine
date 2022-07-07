#pragma once

#include "GL/glew.h"

namespace buf
{
class TestComponent
{
    public:

        GLuint     buffer = 0;
        GLintptr   index = 0;
        GLenum     target = 0;
        GLsizeiptr bytes = 0;
        GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT;
        char*      persistent_map = nullptr;
};
}; // namespace buf