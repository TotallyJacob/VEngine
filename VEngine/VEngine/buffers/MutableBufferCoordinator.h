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

        /* Mutable buffer functions */
        auto calculate_alignment_bytes(GLenum buffer_type, size_t bytes_per_buffer) const -> GLsizeiptr
        {
            GLsizeiptr alignment_per_buffer = 0;

            // query the alignment
            GLint alignment = 0;
            switch (buffer_type)
            {
                case GL_SHADER_STORAGE_BUFFER:
                    alignment_per_buffer = m_ssbo_alignment;
                    break;

                case GL_UNIFORM_BUFFER:
                    alignment_per_buffer = m_ubo_alignment;
                    break;
            }

            // calculate the alignment
            if (alignment > 0)
            {
                // float casts
                size_t alignment_f = static_cast<size_t>(alignment);
                alignment_per_buffer = ceil(bytes_per_buffer / alignment_f) * alignment_f - bytes_per_buffer;
            }

            return alignment_per_buffer;
        }

        template <typename DATA_TYPE>
        auto calculate_buffer_bytes(GLenum buffer_type, PartitionId num_partitions, unsigned int num_elements) const -> GLsizeiptr
        {
            GLsizeiptr bytes = 0;

            GLsizeiptr bytes_per_buffer = sizeof(DATA_TYPE) * num_elements;
            GLsizeiptr alignment_bytes = calculate_alignment_bytes(buffer_type, static_cast<size_t>(bytes_per_buffer));

            bytes = (bytes_per_buffer + alignment_bytes) * num_partitions;

            return bytes;
        }

        //
        void get_buffer_partitions(BufferId buffer, PartitionId first_id, PartitionId end_id)
        {
            m_partition_manager->partition_data(buffer, end_id, first_id);
            end_id = end_id + first_id; // adding num_partitions to first partition id
        }


        /* BCS functions*/

        [[nodiscard]] auto create_buffer(unsigned int num_partitions) -> BufferId;
        void               destroy_buffer(BufferId buffer);


        // Registering components
        template <typename Initial, typename... T>
        void register_buffer_components();

        template <typename Initial, typename... T>
        void register_partition_components(const Id preallocation_size = preallocation_num_partitions());

        // Adding components
        template <typename I, typename... T>
        void add_buffer_components(BufferId buffer);

        template <typename I, typename... T>
        void add_buffer_components(BufferId buffer, I initial, T... other);

        template <typename I, typename... T>
        void add_buffer_partition_components(BufferId buffer);

        template <typename I, typename... T>
        void add_buffer_partition_components(BufferId buffer, I initial, T... other);


        // getting component data

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


        /* BCS functions */
        template <typename T>
        void add_buffer_component(BufferId buffer, T component = {});

        template <typename T>
        void add_buffer_partition_component(BufferId buffer, T component = {});


        static constexpr auto preallocation_num_partitions() -> PartitionId
        {
            return num_buffers * 3;
        }

        // Alignment
        GLint m_ssbo_alignment = 0;
        GLint m_ubo_alignment = 0;

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