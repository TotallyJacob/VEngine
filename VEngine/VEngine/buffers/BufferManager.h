#pragma once

#include <assert.h>
#include <iostream>
#include <queue>
#include <vector>

#include "../Logger.hpp"
#include "MutableBufferOptions.hpp"

namespace buf
{

#define BUFFER_MANAGER_IDENTIFIER <num_buffers>
#define BUFFER_MANAGER_TEMPLATE   template <size_t num_buffers>

BUFFER_MANAGER_TEMPLATE
class BufferManager
{
    public:

        BufferManager();

        [[nodiscard]] auto create_buffer() -> BufferId;
        void               destroy_buffer(BufferId id);

        void               set_signature(BufferId id, Signature signature);
        [[nodiscard]] auto get_signature(BufferId id) -> Signature;

    private:

        std::queue<BufferId>               m_avalible_buffer_ids{};
        std::array<Signature, num_buffers> m_signatures{};
        uint32_t                           m_living_buffer_count{};
};

}; // namespace buf

#include "BufferManager.ipp"

#undef BUFFER_MANAGER_IDENTIFIER
#undef BUFFER_MANAGER_TEMPLATE