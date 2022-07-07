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

        PartitionManager()
        {
            for (PartitionId i = 0; i < prealloc_partition_ids; i++)
            {
                m_avalible_partition_ids.push(i);
            }

            m_buffer_id_to_partitions.reserve(prealloc_partition_ids);
            m_signatures.resize(prealloc_partition_ids);
        }

        void add_partitions(BufferId id, unsigned int num_partitions)
        {
            if (num_partitions == 0)
            {
                VE_LOG_ERROR("A buffer must have atleast 1 partition, thus cannot make buffer partition." << __FILE__ << " at "
                                                                                                          << __LINE__);
                return;
            }

            BufferPartitionData data = {};
            data.num_partitions = num_partitions;
            data.first_partition = create_partition();

            // Create partitions for all remaining partitions
            for (unsigned int partitions_created = 1; partitions_created < num_partitions; partitions_created++)
            {
                create_partition();
            }

            m_buffer_id_to_partitions.emplace(id, data);
        }

        void destroy_partitions(BufferId id)
        {
            unsigned int num_partitions = 0;
            PartitionId  first_partition = 0;

            if (!partition_data(id, num_partitions, first_partition))
            {
                VE_LOG_ERROR("Could not destory entity. " << __FILE__ << " at " << __LINE__);
                return;
            }

            for (unsigned int i = 0; i < num_partitions; i++)
            {
                destroy_partition(first_partition + i);
            }

            m_buffer_id_to_partitions.erase(id);
        }


        void set_signature(PartitionId id, Signature signature)
        {
            if (m_signatures.size() <= id)
            {
                size_t new_size = id;
                m_signatures.resize(new_size + 1);
            }

            m_signatures.at(id) = signature;
        }

        [[nodiscard]] Signature get_signature(PartitionId id)
        {
            if (m_signatures.size() <= id)
            {
                VE_LOG_ERROR("Partition id too large, cannot return partition. " << __FILE__ << " at " << __LINE__);
                std::abort();
            }

            return m_signatures.at(id);
        }

        bool partition_data(BufferId id, unsigned int& num_partitions, PartitionId& first_partition_id)
        {
            if (m_buffer_id_to_partitions.find(id) == m_buffer_id_to_partitions.end())
            {
                VE_LOG_ERROR("Cannot find partition data -> it doesn't exist. " << __FILE__ << " at " << __LINE__);
                return false;
            }

            auto [_num_partitions, _first_partition] = m_buffer_id_to_partitions.at(id);
            num_partitions = _num_partitions;
            first_partition_id = _first_partition;

            return true;
        }

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

        PartitionId create_partition()
        {
            PartitionId id = 0;

            if (m_avalible_partition_ids.size() != 0) // When we can reuse ids
            {
                id = m_avalible_partition_ids.front();
                m_avalible_partition_ids.pop();
            }
            else // When we have to create a new id
            {
                id = m_new_largest_partition_id;
            }

            m_new_largest_partition_id++;

            return id;
        }

        void destroy_partition(PartitionId id)
        {
            if (m_new_largest_partition_id <= id)
            {
                VE_LOG_ERROR("PartitionId id too large, cannot destroy partition. " << __FILE__ << " at " << __LINE__);
                return;
            }

            m_avalible_partition_ids.push(id);
        }
};
}; // namespace buf