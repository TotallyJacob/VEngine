#pragma once

#include "EntityManager.h"

namespace vengine
{

class IComponentArray
{
    public:

        virtual ~IComponentArray() = default;
        virtual void entity_destroyed(Entity entity) = 0;
};

}; // namespace vengine
