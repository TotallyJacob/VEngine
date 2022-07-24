#pragma once

#include "GL/glew.h"

#include <atomic>

namespace vengine
{
template <bool atomic>
class BufferBindingData;

template <>
class BufferBindingData<true>
{
    public:

        std::atomic<bool>     m_data_changed = {false};
        std::atomic<GLintptr> m_offset = {0};
};

template <>
class BufferBindingData<false>
{
    public:

        bool     m_data_changed = false;
        GLintptr m_offset = 0;
};

} // namespace vengine