#pragma once

namespace buf
{
void MutableBufferCoordinator::init()
{
    m_buffer_component_manager = std::make_unique<CompManager>();
    m_partition_component_manager = std::make_unique<CompManager>();

    m_buffer_manager = std::make_unique<BufferManager>();
    m_partition_manager = std::make_unique<PartitionManager>();


    m_buffer_system_manager = std::make_unique<SystemManager>();
    m_partition_system_manager = std::make_unique<SystemManager>();
}
[[nodiscard]] BufferId MutableBufferCoordinator::create_buffer(unsigned int num_partitions)
{
    auto buffer_id = m_buffer_manager->create_buffer();

    m_partition_manager->create_partitions(buffer_id, num_partitions);

    return buffer_id;
}
void MutableBufferCoordinator::destroy_buffer(BufferId buffer)
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
void MutableBufferCoordinator::register_buffer_component(const Id preallocation_size)
{
    m_buffer_component_manager->register_components<T>(preallocation_size);
}
template <typename T>
void MutableBufferCoordinator::register_partition_component(const Id preallocation_size)
{
    m_partition_component_manager->register_components<T>(preallocation_size);
}

template <typename T>
void MutableBufferCoordinator::add_buffer_component(BufferId buffer, T component)
{
    m_buffer_component_manager->add_component<T>(buffer, component);

    auto signature = m_buffer_manager->get_signature(buffer);
    signature.set(m_buffer_component_manager->get_component_type<T>(), true);
    m_buffer_manager->set_signature(buffer, signature);

    m_buffer_system_manager->id_signature_changed(buffer, signature);
}

template <typename T>
void MutableBufferCoordinator::add_buffer_partition_component(BufferId buffer, T component)
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
T& MutableBufferCoordinator::get_buffer_component(Id entity)
{
    return m_buffer_component_manager->get_component<T>(entity);
}

template <typename T>
ComponentType MutableBufferCoordinator::get_buffer_component_type()
{
    return m_buffer_component_manager->get_component_type<T>();
}

template <typename T>
T& MutableBufferCoordinator::get_partition_component(Id entity)
{
    return m_partition_component_manager->get_component<T>(entity);
}

template <typename T>
ComponentType MutableBufferCoordinator::get_partition_component_type()
{
    return m_partition_component_manager->get_component_type<T>();
}


// System methods
template <typename T>
std::shared_ptr<T> MutableBufferCoordinator::register_buffer_system()
{
    return m_buffer_system_manager->register_system<T>();
}

template <typename T>
void MutableBufferCoordinator::set_buffer_system_signature(Signature signature)
{
    m_buffer_system_manager->set_signature<T>(signature);
}

template <typename T>
std::shared_ptr<T> MutableBufferCoordinator::register_partition_system()
{
    return m_partition_system_manager->register_system<T>();
}

template <typename T>
void MutableBufferCoordinator::set_partition_system_signature(Signature signature)
{
    m_partition_system_manager->set_signature<T>(signature);
}
}; // namespace buf