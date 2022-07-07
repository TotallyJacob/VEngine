#pragma once

#include "GL/glew.h"

#include "MutableBufferCoordinator.h"
#include "System.h"
#include "TestComponent.h"

namespace buf
{

class TestSystem : public System
{
    public:

        MutableBufferCoordinator& mbc;

        TestSystem(MutableBufferCoordinator& mbc) : mbc(mbc)
        {
        }

        void set_initial_buffer_data(BufferId buffer, GLsizeiptr buf_len, GLenum buf_target)
        {
            auto collection = m_ids.find(buffer);
            for (auto it = collection; it != m_ids.end(); it++)
            {
                auto component = mbc.get_buffer_component<TestComponent>(*it);
                component.bytes = buf_len;
                component.target = buf_target;
            }
        }

        void init_buffers()
        {
            for (auto& buffer : m_ids)
            {
                auto comp = mbc.get_buffer_component<TestComponent>(buffer);

                glCreateBuffers(1, &comp.buffer); // Could actually be made better

                glNamedBufferStorage(comp.buffer, comp.bytes, nullptr, comp.flags); // allocating storage

                comp.persistent_map =
                    static_cast<char*>(glMapNamedBufferRange(comp.buffer, comp.index, comp.bytes, comp.flags)); // persistent mapping

                glBindBufferRange(comp.target, 0, comp.buffer, 0, comp.bytes); // for use in shaders
            }
        }
};

}; // namespace buf
