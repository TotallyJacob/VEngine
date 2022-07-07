#pragma once

#include "GL/glew.h"
#include <vector>

#include <atomic>
#include <mutex>

namespace vengine
{
class BufferBinding
{
    public:
    private:

        std::mutex m_mutex = {};
};
} // namespace vengine