#pragma once

namespace buf
{
MBC_TEMPLATE
void MutableBufferCoordinator MBC_IDENTIFIER::init()
{
    m_buffer_component_manager = std::make_unique<CompManager<StaticCompData, num_buffers>>();
    m_partition_component_manager = std::make_unique<CompManager<CompData>>();

    m_buffer_manager = std::make_unique<BufferManager<num_buffers>>();
    m_partition_manager = std::make_unique<PartitionManager>();


    m_buffer_system_manager = std::make_unique<BufferSystemManager>();
    m_partition_system_manager = std::make_unique<BufferSystemManager>();


    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &m_ssbo_alignment);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &m_ubo_alignment);
}

MBC_TEMPLATE
[[nodiscard]] BufferId MutableBufferCoordinator MBC_IDENTIFIER::create_buffer(unsigned int num_partitions)
{
    auto buffer_id = m_buffer_manager->create_buffer();

    m_partition_manager->create_partitions(buffer_id, num_partitions);

    return buffer_id;
}

MBC_TEMPLATE
void MutableBufferCoordinator MBC_IDENTIFIER::destroy_buffer(BufferId buffer)
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

MBC_TEMPLATE
template <typename Initial, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::register_buffer_components()
{
    m_buffer_component_manager->register_component<Initial>();
    (m_buffer_component_manager->register_component<T>(), ...);
}

MBC_TEMPLATE
template <typename Initial, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::register_partition_components(const Id preallocation_size)
{
    m_partition_component_manager->register_component<Initial>(preallocation_size);
    (m_partition_component_manager->register_component<T>(preallocation_size), ...);
}

MBC_TEMPLATE
template <typename T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_component(BufferId buffer, T component)
{
    m_buffer_component_manager->add_component<T>(buffer, component);

    auto signature = m_buffer_manager->get_signature(buffer);
    signature.set(m_buffer_component_manager->get_component_type<T>(), true);
    m_buffer_manager->set_signature(buffer, signature);

    m_buffer_system_manager->id_signature_changed(buffer, signature);
}

MBC_TEMPLATE
template <typename I, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_components(BufferId buffer)
{
    add_buffer_component<I>(buffer);
    (add_buffer_component<T>(buffer), ...);
}

MBC_TEMPLATE
template <typename I, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_partition_components(BufferId buffer)
{
    add_buffer_partition_component<I>(buffer);
    (add_buffer_partition_component<T>(buffer), ...);
}

MBC_TEMPLATE
template <typename I, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_components(BufferId buffer, I initial, T... other)
{
    add_buffer_component<I>(buffer, initial);
    (add_buffer_component<T>(buffer, std::forward<T>(other)), ...);
}

MBC_TEMPLATE
template <typename I, typename... T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_partition_components(BufferId buffer, I initial, T... other)
{
    add_buffer_partition_component<I>(buffer, initial);
    (add_buffer_partition_component<T>(buffer, std::forward<T>(other)), ...);
}



MBC_TEMPLATE
template <typename T>
void MutableBufferCoordinator MBC_IDENTIFIER::add_buffer_partition_component(BufferId buffer, T component)
{
    m_partition_manager->for_each_partition(buffer,
                                            [&](PartitionId id)
                                            {
                                                m_partition_component_manager->add_component<T>(id, component);

                                                auto signature = m_partition_manager->get_signature(id);
                                                signature.set(m_partition_component_manager->get_component_type<T>(), true);

                                                m_partition_manager->set_signature(id, signature);
                                                m_partition_system_manager->id_signature_changed(id, signature);
                                            });
}



MBC_TEMPLATE
template <typename T>
T& MutableBufferCoordinator MBC_IDENTIFIER::get_buffer_component(Id id)
{
    return m_buffer_component_manager->get_component<T>(id);
}

MBC_TEMPLATE
template <typename T>
ComponentType MutableBufferCoordinator MBC_IDENTIFIER::get_buffer_component_type()
{
    return m_buffer_component_manager->get_component_type<T>();
}

MBC_TEMPLATE
template <typename T>
T& MutableBufferCoordinator MBC_IDENTIFIER::get_partition_component(Id id)
{
    return m_partition_component_manager->get_component<T>(id);
}

MBC_TEMPLATE
template <typename T>
ComponentType MutableBufferCoordinator MBC_IDENTIFIER::get_partition_component_type()
{
    return m_partition_component_manager->get_component_type<T>();
}

MBC_TEMPLATE
// System methods
template <typename T>
std::shared_ptr<T> MutableBufferCoordinator MBC_IDENTIFIER::register_buffer_system()
{
    return m_buffer_system_manager->register_system<T>();
}

MBC_TEMPLATE
template <typename T>
void MutableBufferCoordinator MBC_IDENTIFIER::set_buffer_system_signature(Signature signature)
{
    m_buffer_system_manager->set_signature<T>(signature);
}

MBC_TEMPLATE
template <typename T>
std::shared_ptr<T> MutableBufferCoordinator MBC_IDENTIFIER::register_partition_system()
{
    return m_partition_system_manager->register_system<T>();
}

MBC_TEMPLATE
template <typename T>
void MutableBufferCoordinator MBC_IDENTIFIER::set_partition_system_signature(Signature signature)
{
    m_partition_system_manager->set_signature<T>(signature);
}


}; // namespace buf