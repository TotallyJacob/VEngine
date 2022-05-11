#pragma once

#include <array>
#include <assert.h>
#include <bitset>
#include <iostream>
#include <memory>
#include <queue>

namespace vengine
{

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 100000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager
{
    public:

        EntityManager();

        [[nodiscard]] Entity create_entity();
        void                 destroy_entity(Entity entity);

        // Setters
        void                    set_signature(Entity entity, Signature signature);
        [[nodiscard]] Signature get_signature(Entity entity);

    private:

        std::queue<Entity>                  m_avalible_entities{};
        std::array<Signature, MAX_ENTITIES> m_signatures{};
        uint32_t                            m_living_entity_count{};
};

}; // namespace vengine
