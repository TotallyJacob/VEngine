#pragma once

#include "GL/glew.h"

#include "../Logger.hpp"
#include "../Timer.h"
#include <assert.h>
#include <iostream>
#include <vector>

namespace vengine
{

class SyncBuffer
{

    public:

        SyncBuffer() = default;
        SyncBuffer(unsigned int buffer_size);
        void init(unsigned int buffer_size);

        void insert_sync(const unsigned int sync_index);
        void insert_all_syncs();

        void delete_sync(const unsigned int sync_index); // allows an already invalid/deleted sync.
        void delete_all_syncs();

        auto sync_at(const unsigned int sync_index) -> GLsync;
        auto num_syncs() -> const size_t;
        auto sync_invalid(const unsigned int sync_index) -> bool;

    private:

        constexpr static GLsync m_invalid_sync = 0;
        std::vector<GLsync>     m_sync_buf = {};

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