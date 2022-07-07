#pragma once

#include <bitset>
#include <iostream>
#include <limits>

namespace buf
{
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 10;
using Signature = std::bitset<MAX_COMPONENTS>;

using Id = std::uint32_t;
using BufferId = Id;
using PartitionId = Id;


constexpr static BufferId     prealloc_buffer_ids = 100;
constexpr static unsigned int partition_id_product_factor = 3; // How many more partitions there are per buffer

constexpr static PartitionId prealloc_partition_ids =
    (std::numeric_limits<std::uint32_t>::max() / partition_id_product_factor <= prealloc_buffer_ids)
        ? std::numeric_limits<std::uint32_t>::max()
        : prealloc_buffer_ids * partition_id_product_factor; // To avoid overflow lol

}; // namespace buf