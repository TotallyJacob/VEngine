#pragma once

#include "MutableBufferOptions.hpp"
#include <set>

namespace buf
{
class System
{
    public:

        std::set<Id> m_ids{};
};
} // namespace buf