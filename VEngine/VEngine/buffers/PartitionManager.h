#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "../Logger.hpp"
#include "MutableBufferOptions.hpp"

namespace buf
{
class PartitionManager
{
    public:

        PartitionManager();

        void create_partitions(BufferId id, unsigned int num_partitions);
        void destroy_partitions(BufferId id);

        void               set_signature(PartitionId id, Signature signature);
        [[nodiscard]] auto get_signature(PartitionId id) -> Signature;

        bool partition_data(BufferId id, unsigned int& num_partitions, PartitionId& first_partition_id);

        template <typename F>
        void for_each_partition(BufferId id, F&& lambda)
        {
            unsigned int num_partitions = 0;
            PartitionId  first_partition = 0;
            if (partition_data(id, num_partitions, first_partition))
            {
                for (PartitionId id = first_partition; id < first_partition + num_partitions; id++)
                {
                    lambda(id);
                }
            }
        }

    private:

        struct BufferPartitionData
        {
                unsigned int num_partitions = 0;
                PartitionId  first_partition = 0;
        };

        std::unordered_map<BufferId, BufferPartitionData> m_buffer_id_to_partitions{};
        std::queue<PartitionId>                           m_avalible_partition_ids{};
        std::vector<Signature>                            m_signatures{};
        uint32_t                                          m_new_largest_partition_id{};

        auto create_partition() -> PartitionId;
        void destroy_partition(PartitionId id);
};
}; // namespace buf