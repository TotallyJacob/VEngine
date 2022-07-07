#pragma once

#include "GL/glew.h"

#include <array>
#include <atomic>

#include "../Logger.hpp"
#include "../ThreadLicense.h"
#include "ConcSyncInserter.h"

namespace vengine
{

#define CONC_SYNC_BUFFER_IDENTIFIER <buffer_size, thread_license>
#define CONC_SYNC_BUFFER_TEMPLATE   template <unsigned int buffer_size, bool thread_license>

CONC_SYNC_BUFFER_TEMPLATE
class ConcSyncBuffer
{
    public:

        ConcSyncBuffer() = default;

        void make_sync_inserter(const std::shared_ptr<ConcSyncInserter<buffer_size, thread_license>>& sync_inserter);
        void delete_sync_at(unsigned int sync_index);
        void delete_all_syncs();

        auto           sync_at(unsigned int sync_index) -> std::atomic<GLsync>&;
        auto           sync_invalid(const unsigned int sync_index) -> bool;
        constexpr auto num_syncs() -> const unsigned int;

    private:

        constexpr static GLsync                                        m_invalid_sync = 0;
        std::shared_ptr<ConcSyncInserter<buffer_size, thread_license>> m_sync_inserter = {};
};

}; // namespace vengine


#include "ConcSyncBuffer.hpp"

#undef CONC_SYNC_BUFFER_IDENTIFIER
#undef CONC_SYNC_BUFFER_TEMPLATE