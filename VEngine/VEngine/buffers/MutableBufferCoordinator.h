#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "BufferManager.h"
#include "BufferSystemManager.h"
#include "CompManager.h"
#include "MutableBufferOptions.hpp"
#include "PartitionManager.h"
#include "StaticCompData.h"

namespace buf
{

#define MBC_IDENTIFIER <num_buffers>
#define MBC_TEMPLATE   template <size_t num_buffers>

MBC_TEMPLATE
class MutableBufferCoordinator
{

    public:

        MutableBufferCoordinator() = default;

        void init();

        [[nodiscard]] auto create_buffer(unsigned int num_partitions) -> BufferId;
        void               destroy_buffer(BufferId buffer);

        template <typename T>
        void register_buffer_component();

        template <typename T>
        void register_partition_component(const Id preallocation_size = preallocation_num_partitions());

        template <typename T>
        void add_buffer_component(BufferId buffer, T component);

        template <typename T>
        void add_buffer_partition_component(BufferId buffer, T component);

        template <typename T>
        auto get_buffer_component(Id entity) -> T&;

        template <typename T>
        auto get_buffer_component_type() -> ComponentType;

        template <typename T>
        auto get_partition_component(Id entity) -> T&;

        template <typename T>
        ComponentType get_partition_component_type();


        // System methods
        template <typename T>
        auto register_buffer_system() -> std::shared_ptr<T>;

        template <typename T>
        void set_buffer_system_signature(Signature signature);

        template <typename T>
        auto register_partition_system() -> std::shared_ptr<T>;

        template <typename T>
        void set_partition_system_signature(Signature signature);


    private:

        static constexpr auto preallocation_num_partitions() -> PartitionId
        {
            return num_buffers * 3;
        }

        // buffer
        std::unique_ptr<BufferManager<num_buffers>>               m_buffer_manager{};
        std::unique_ptr<BufferSystemManager>                      m_buffer_system_manager{};
        std::unique_ptr<CompManager<StaticCompData, num_buffers>> m_buffer_component_manager{};

        // partition
        std::unique_ptr<PartitionManager>      m_partition_manager{};
        std::unique_ptr<BufferSystemManager>   m_partition_system_manager{};
        std::unique_ptr<CompManager<CompData>> m_partition_component_manager{};
};
}; // namespace buf

#include "MutableBufferCoordinator.ipp"

#undef MBC_IDENTIFIER
#undef MBC_TEMPLATE