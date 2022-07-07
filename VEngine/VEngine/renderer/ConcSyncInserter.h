#pragma once

#include "GL/glew.h"

#include <array>
#include <atomic>

namespace vengine
{

#define CONC_SYNC_INSERTER_IDENTIFIER <buffer_size, thread_license>
#define CONC_SYNC_INSERTER_TEMPLATE   template <unsigned int buffer_size, bool thread_license>

CONC_SYNC_INSERTER_TEMPLATE
class ConcSyncInserter
{
    public:

        ConcSyncInserter() = default;
        ConcSyncInserter(bool init);
        void init();

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
        std::array<std::atomic<GLsync>, buffer_size> m_syncs = {0};

    public:

        template <bool print_if_sync_fail, bool print_sync_time>
        void standard_wait_sync(const GLsync& sync, const std::string& print_sync_message = "", const GLuint64 wait_time = 100000)
        {
            // Timer init
            LARGE_INTEGER initial_time;
            if constexpr (print_sync_time)
            {
                Timer timer;
                timer.currentTime(&initial_time);
            }

            // Syncing
            bool   con = false;
            GLenum result;
            while (!con)
            {
                result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, wait_time);

                switch (result)
                {
                    case GL_WAIT_FAILED:
                        con = true;
                        break;

                    case GL_CONDITION_SATISFIED:
                        con = true;
                        break;

                    case GL_ALREADY_SIGNALED:
                        con = true;
                        break;
                }
            }

            // Printing if sync failed
            if constexpr (print_if_sync_fail)
            {
                if (result == GL_WAIT_FAILED)
                {
                    VE_LOG_DEBUG(print_sync_message << "Not synced.");
                }
            }

            // Time final
            if constexpr (print_sync_time)
            {
                Timer  timer;
                double elapsed = 0;
                timer.getMsElapsed(initial_time, elapsed);

                VE_LOG_DEBUG(print_sync_message << "sync time: " << elapsed << " ms.");
            }
        }
};

} // namespace vengine

#include "ConcSyncInserter.hpp"

#undef CONC_SYNC_INSERTER_IDENTIFIER
#undef CONC_SYNC_INSERTER_TEMPLATE