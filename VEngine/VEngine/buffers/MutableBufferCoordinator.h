#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "BufferManager.h"
#include "CompManager.h"
#include "MutableBufferOptions.hpp"
#include "PartitionManager.h"
#include "SystemManager.h"

namespace buf
{

class MutableBufferCoordinator
{

    public:

        MutableBufferCoordinator() = default;

        void init()
        {
            m_buffer_component_manager = std::make_unique<CompManager>();
            m_partition_component_manager = std::make_unique<CompManager>();

            m_buffer_manager = std::make_unique<BufferManager>();
            m_partition_manager = std::make_unique<PartitionManager>();


            m_buffer_system_manager = std::make_unique<SystemManager>();
            m_partition_system_manager = std::make_unique<SystemManager>();
        }
        [[nodiscard]] BufferId create_buffer(unsigned int num_partitions)
        {
            auto buffer_id = m_buffer_manager->create_buffer();

            m_partition_manager->add_partitions(buffer_id, num_partitions);

            return buffer_id;
        }
        void destroy_buffer(BufferId buffer)
        {

            m_partition_manager->for_each_partition(buffer,
                                                    [&](unsigned int id)
                                                    {
                                                        m_partition_system_manager->id_destroyed(id);
                                                        m_partition_component_manager->id_destroyed(id);
                                                    });
            m_partition_manager->destroy_partitions(buffer);


            m_buffer_system_manager->id_destroyed(buffer);
            m_buffer_component_manager->id_destroyed(buffer);
            m_buffer_manager->destroy_buffer(buffer);
        }


        template <typename T>
        void register_buffer_component(const Id preallocation_size)
        {
            m_buffer_component_manager->register_components<T>(preallocation_size);
        }
        template <typename T>
        void register_partition_component(const Id preallocation_size)
        {
            m_partition_component_manager->register_components<T>(preallocation_size);
        }

        template <typename T>
        void add_buffer_component(BufferId buffer, T component)
        {
            m_buffer_component_manager->add_component<T>(buffer, component);

            auto signature = m_buffer_manager->get_signature(buffer);
            signature.set(m_buffer_component_manager->get_component_type<T>(), true);
            m_buffer_manager->set_signature(buffer, signature);

            m_buffer_system_manager->id_signature_changed(buffer, signature);
        }

        template <typename T>
        void add_buffer_partition_component(BufferId buffer, T component)
        {
            m_partition_manager->for_each_partition(buffer,
                                                    [&](PartitionId id)
                                                    {
                                                        m_partition_component_manager->add_component<T>(id, component);
                                                        m_partition_manager->set_signature(id, signature);

                                                        auto signature = m_partition_manager->get_signature(id);
                                                        signature.set(m_partition_component_manager->get_component_type<T>(), true);

                                                        m_partition_manager->set_signature(id, signature);
                                                        m_partition_system_manager->id_signature_changed(id, signature);
                                                    });
        }




        template <typename T>
        T& get_buffer_component(Id entity)
        {
            return m_buffer_component_manager->get_component<T>(entity);
        }

        template <typename T>
        ComponentType get_buffer_component_type()
        {
            return m_buffer_component_manager->get_component_type<T>();
        }

        template <typename T>
        T& get_partition_component(Id entity)
        {
            return m_partition_component_manager->get_component<T>(entity);
        }

        template <typename T>
        ComponentType get_partition_component_type()
        {
            return m_partition_component_manager->get_component_type<T>();
        }


        // System methods
        template <typename T>
        std::shared_ptr<T> register_buffer_system()
        {
            return m_buffer_system_manager->register_system<T>();
        }

        template <typename T>
        void set_buffer_system_signature(Signature signature)
        {
            m_buffer_system_manager->set_signature<T>(signature);
        }

        template <typename T>
        std::shared_ptr<T> register_partition_system()
        {
            return m_partition_system_manager->register_system<T>();
        }

        template <typename T>
        void set_partition_system_signature(Signature signature)
        {
            m_partition_system_manager->set_signature<T>(signature);
        }


    private:

        // buffer
        std::unique_ptr<BufferManager> m_buffer_manager{};
        std::unique_ptr<SystemManager> m_buffer_system_manager{};
        std::unique_ptr<CompManager>   m_buffer_component_manager{};

        // partition
        std::unique_ptr<PartitionManager> m_partition_manager{};
        std::unique_ptr<SystemManager>    m_partition_system_manager{};
        std::unique_ptr<CompManager>      m_partition_component_manager{};
};
}; // namespace buf
