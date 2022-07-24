#pragma once

#include "GL/glew.h"

#include "MutableBufferCoordinator.h"
#include "System.h"
#include "TestComponent.h"

#include "../CourierBuffer.h"
#include "../renderer/PersistentMapBufferUtil.h"

#include <array>

namespace buf
{

template <size_t num_buffers>
class TestSystem : public System
{
    public:

        MutableBufferCoordinator<num_buffers>* mbc = nullptr;

        TestSystem() = default;

        void init(MutableBufferCoordinator<num_buffers>* mbc)
        {
            this->mbc = mbc;
        }

        /* void set_initial_buffer_data(BufferId buffer, GLsizeiptr buf_len, GLenum buf_target)
        {
            auto collection = m_ids.find(buffer);
            for (auto it = collection; it != m_ids.end(); it++)
            {
                auto component = mbc->get_buffer_component<TestBC>(*it);
                component.bytes = buf_len;
                component.target = buf_target;
            }
        }*/

        void init_buffers()
        {
            for (auto& buffer : m_ids)
            {
                auto& comp = mbc->get_buffer_component<TestBC>(buffer);
                auto& flags = mbc->get_buffer_component<FlagsBC>(buffer);

                glCreateBuffers(1, &comp.buffer);

                glNamedBufferStorage(comp.buffer, comp.bytes, nullptr, flags.flags); // allocating storage

                comp.persistent_map =
                    static_cast<char*>(glMapNamedBufferRange(comp.buffer, 0, comp.bytes, flags.flags)); // persistent mapping
            }
        }
};

template <size_t num_buffers>
class TestBufferModifySystem : public System
{
    public:

        MutableBufferCoordinator<num_buffers>* mbc = nullptr;

        TestBufferModifySystem() = default;

        void init(MutableBufferCoordinator<num_buffers>* mbc)
        {
            this->mbc = mbc;
        }

        template <typename DATA_TYPE>
        [[nodiscard]] auto make_merge_array(BufferId id, size_t alignment_bytes_per_buf) -> vengine::CourierBuffer<DATA_TYPE>
        {
            unsigned int                      num_elements = mbc->get_buffer_component<ElementsBC>(id).num_elements;
            vengine::CourierBuffer<DATA_TYPE> courier_buffer(num_elements, alignment_bytes_per_buf);
            m_data.at(id) = (courier_buffer.get_byte_array().get_data_bytes());

            return courier_buffer;
        }


        void merge_data()
        {
            for (const BufferId& buf : m_ids)
            {
                const auto& ptr = m_data.at(buf);
                if (ptr == nullptr)
                {
                    continue;
                }

                unsigned int num_elements = mbc->get_buffer_component<ElementsBC>(buf).num_elements;
                TestBC&      map = mbc->get_buffer_component<TestBC>(buf);
                vengine::util::buf_memcpy<char>(map.persistent_map, ptr, map.bytes, 0);
            }
        }

    private:

        std::array<char*, num_buffers> m_data = {nullptr};
};

}; // namespace buf
