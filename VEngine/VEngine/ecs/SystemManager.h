#pragma once

#include <assert.h>
#include <iostream>
#include <memory>
#include <unordered_map>

// My includes
#include "EntityManager.h"
#include "System.h"

namespace vengine
{

class SystemManager
{
    public:

        template <typename T>
        std::shared_ptr<T> register_system()
        {
            const char* typeName = typeid(T).name();

            assert(m_systems.find(typeName) == m_systems.end() && "Registering a system more then once.");

            auto system = std::make_shared<T>();
            m_systems.insert({typeName, system});

            return system;
        }

        template <typename T>
        void set_signature(Signature signature)
        {
            const char* typeName = typeid(T).name();

            assert(m_systems.find(typeName) != m_systems.end() && "System used before registering");

            m_signatures.insert({typeName, signature});
        }

        void entity_destroyed(Entity entity);
        void entity_signature_changed(Entity entity, Signature newSignature);

    private:

        std::unordered_map<const char*, Signature>               m_signatures{};
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
};

}; // namespace vengine
