#pragma once

#include <iostream>
#include <set>

#include "EntityManager.h"

namespace vengine
{
class System
{
    public:

        std::set<Entity> m_entities;
};
}; // namespace vengine
