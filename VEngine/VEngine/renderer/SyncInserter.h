#pragma once

#include "GL/glew.h"

#include <atomic>

namespace vengine
{

class SyncInserter
{
    public:

        SyncInserter() = default;

        // Due to atomic
        SyncInserter(const SyncInserter&) = delete;
        SyncInserter(SyncInserter&&) = delete;

        auto try_insert(unsigned int sync_index) -> bool
        {
            if (m_sync != m_invalid_sync)
            {
                return false;
            }

            if (m_previous_sync_index == sync_index)
            {
                return false;
            }

            m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glFlush();
            m_previous_sync_index = sync_index;
            return true;
        }

        auto sync() -> std::atomic<GLsync>&
        {
            return m_sync;
        }

    private:

        constexpr static GLsync m_invalid_sync = 0;
        unsigned int            m_previous_sync_index = 0;
        std::atomic<GLsync>     m_sync = 0;
};

} // namespace vengine
