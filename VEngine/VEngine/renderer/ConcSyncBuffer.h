#pragma once

#include "GL/glew.h"

#include <array>
#include <atomic>

#include "../Logger.hpp"
#include "../ThreadLicense.h"
#include "ConcSyncInserter.h"

namespace vengine
{

#define CONC_SYNC_BUFFER_IDENTIFIER <num_buffers, thread_license>
#define CONC_SYNC_BUFFER_TEMPLATE   template <unsigned int num_buffers, bool thread_license>

CONC_SYNC_BUFFER_TEMPLATE
class ConcSyncBuffer
{
    public:

        ConcSyncBuffer() = default;

        void make_insereter_thread(const std::shared_ptr<ConcSyncInserter<num_buffers, thread_license>>& sync_inserter);
        void delete_sync_at(unsigned int sync_index);
        void sync_at(unsigned int sync_index)->std::atomic<GLsync>&;

    private:

        std::shared_ptr<ConcSyncInserter<num_buffers, thread_license>> m_sync_inserter = {};
};

}; // namespace vengine


#include "ConcSyncBuffer.hpp"

#undef CONC_SYNC_BUFFER_IDENTIFIER
#undef CONC_SYNC_BUFFER_TEMPLATE