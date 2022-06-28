#pragma once

#include "GL/glew.h"

#include <array>
#include <atomic>

namespace vengine
{

#define CONC_SYNC_INSERTER_IDENTIFIER <num_buffers, thread_license>
#define CONC_SYNC_INSERTER_TEMPLATE   template <unsigned int num_buffers, bool thread_license>

CONC_SYNC_INSERTER_TEMPLATE
class ConcSyncInserter
{
    public:

        ConcSyncInserter();

        // Due to atomic
        ConcSyncInserter(const ConcSyncInserter&) = delete;
        ConcSyncInserter(ConcSyncInserter&&) = delete;

        auto try_insert(unsigned int sync_index) -> bool;
        void delete_sync_at(unsigned int sync_index);
        auto sync_at(unsigned int sync_index) -> std::atomic<GLsync>&;

    private:

        constexpr static GLsync m_invalid_sync = 0;
        unsigned int            m_previous_sync_index = 0;

        ThreadLicense<thread_license>                m_inserter_license = {};
        std::array<std::atomic<GLsync>, num_buffers> m_syncs = {0};
};

} // namespace vengine

#include "ConcSyncInserter.hpp"

#undef CONC_SYNC_INSERTER_IDENTIFIER
#undef CONC_SYNC_INSERTER_TEMPLATE