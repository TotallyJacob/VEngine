#pragma once

#include <iostream>
#include <queue>
#include <vector>

#include "../Logger.hpp"
#include "MutableBufferOptions.hpp"

namespace buf
{

class BufferManager
{
    public:

        BufferManager();

        [[nodiscard]] auto create_buffer() -> BufferId;
        void               destroy_buffer(BufferId id);

        void               set_signature(BufferId id, Signature signature);
        [[nodiscard]] auto get_signature(BufferId id) -> Signature;

    private:

        std::queue<BufferId>   m_avalible_buffer_ids{};
        std::vector<Signature> m_signatures{};
        uint32_t               m_new_largest_buffer_id{};
};

}; // namespace buf