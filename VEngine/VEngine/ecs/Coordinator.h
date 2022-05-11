#pragma once

#include <assert.h>
#include <iostream>
#include <memory>

// My includes
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"


namespace vengine
{

class Coordinator
{
    public:

        void   init();
        Entity create_entity();
        void   destroy_entity(Entity entity);


        template <typename T>
        void register_component()
        {
            m_component_manager->register_components<T>();
        }

        template <typename T>
        void add_component(Entity entity, T component)
        {
            m_component_manager->add_component<T>(entity, component);

            auto signature = m_entity_manager->get_signature(entity);
            signature.set(m_component_manager->get_component_type<T>(), true);
            m_entity_manager->set_signature(entity, signature);

            m_system_manager->entity_signature_changed(entity, signature);
        }

        template <typename T>
        T& get_component(Entity entity)
        {
            return m_component_manager->get_component<T>(entity);
        }

        template <typename T>
        ComponentType get_component_type()
        {
            return m_component_manager->get_component_type<T>();
        }


        // System methods
        template <typename T>
        std::shared_ptr<T> register_system()
        {
            return m_system_manager->register_system<T>();
        }

        template <typename T>
        void set_system_signature(Signature signature)
        {
            m_system_manager->set_signature<T>(signature);
        }

    private:

        std::unique_ptr<ComponentManager> m_component_manager;
        std::unique_ptr<EntityManager>    m_entity_manager;
        std::unique_ptr<SystemManager>    m_system_manager;
};

}; // namespace vengine
