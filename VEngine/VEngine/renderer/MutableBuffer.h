#pragma once

#include "GL/glew.h"

#include "../ByteArray.h"
#include "../CourierBuffer.h"
#include "../Logger.hpp"
#include "../Timer.h"
#include "IMutableBuffer.h"
#include "PersistentMapBufferUtil.h"
#include "SyncBuffer.h"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <vector>

/* TODO: not swap buffers un-nessesarily*/

namespace vengine
{

enum class SpecialSyncType : unsigned int
{
    NORMAL_SYNC = 0,
    DOUBLE_BUFFER_SPECIAL_SYNC = 1,
    TRIPPLE_BUFFER_SPECIAL_SYNC = 2
};

struct MutableBufferFlags
{
        bool coherent_bit = false;
        bool read_bit = false;
        bool explicit_flush = true;
        bool handle_sync = true;
};

#define MUTABLE_BUFFER_IDENTIFIER <DATA_TYPE, buffer_type, num_buffers, flags>
#define MUTABLE_BUFFER_TEMPLATE                                                                                                            \
    template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers, const MutableBufferFlags flags>


template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers,
          const MutableBufferFlags flags = MutableBufferFlags{}>
class MutableBuffer : public IMutableBuffer
{

    public:

        MutableBuffer() = default;
        MutableBuffer(const unsigned int num_elements, const bool gen = false,
                      const SpecialSyncType specialSyncType = SpecialSyncType::NORMAL_SYNC);

        // Init stuff
        void gen(unsigned int num_elements, const SpecialSyncType specialSyncType = SpecialSyncType::NORMAL_SYNC);

        // Binding stuff
        template <bool delete_readbuf_sync>
        void        swap_buffers();
        inline void delete_buffers();
        inline void bind() const;
        inline void unbind() const;

        // Buffer binding stuff
        void flush_previous_update_buf();

        // Sync stuff
        /* template <bool print_if_sync_fail, bool print_sync_time>
        void standard_wait_sync(const GLsync& sync, const std::string& print_sync_message = "", const GLuint64 wait_time = 100000);

        inline void        delete_fence_syncs();
        inline bool        readbuf_sync_invalid();
        inline bool        updatebuf_sync_invalid();
        inline void        delete_readbuf_sync();
        inline void        insert_sync_on_readbuf();
        inline static bool sync_invalid(const auto& sync);

        auto get_readbuf_sync() -> GLsync&;
        auto get_updatebuf_sync() -> GLsync&;*/


        // useful data functions
        void        set_initial_data(CourierBuffer<DATA_TYPE>& buffer);
        void        set_initial_data(unsigned int indexToChange, DATA_TYPE valueToChangeTo);
        void        set_initial_data(DATA_TYPE* value);
        void        set_initial_data(DATA_TYPE* values, unsigned int numData);
        inline void copy_previous_updatebuf_into_updatebuf();


        // insert
        void set_updatebuf_data(CourierBuffer<DATA_TYPE>& buffer);
        void set_updatebuf_data(DATA_TYPE* data, unsigned int numOfElements, unsigned int startPoint);
        void fill_persistent_map(DATA_TYPE* value_to_fill);

        // Actual mem copy stuff
        void set_persistent_map_data(const unsigned int index, DATA_TYPE* data, unsigned int numOfElements, unsigned int startPoint);
        void fill_persistent_map(const unsigned int index, DATA_TYPE* value_to_fill);

        // msc getters
        auto get_persistent_map(const unsigned int index) -> DATA_TYPE*;
        auto get_updatebuf_persistent_map(const unsigned int index) -> DATA_TYPE*;
        auto get_byte_array() -> ByteArray<DATA_TYPE, num_buffers>&;

        auto           get_updatebuf_index() const -> const unsigned int;
        auto           get_readbuf_index() const -> const unsigned int;
        auto           get_alignment_per_buffer() const -> const size_t;
        auto           get_sizeof_elements_per_buf() const -> const size_t;
        constexpr auto get_buffer_type() const -> const GLenum;
        auto           get_buffer_object() const -> const unsigned int;
        constexpr auto get_num_buffers() const -> const unsigned int;
        auto           get_num_elements() const -> const unsigned int;

    protected:

        // constexpr const static GLsync m_invalid_sync_object = 0;

        // gl arrays
        ByteArray<DATA_TYPE, num_buffers> m_persistent_maps = {};
        // SyncBuffer                        m_sync_buf = {num_buffers};
        // GLsync                            m_fences[num_buffers] = {0};
        BindingData  m_binding_data[num_buffers] = {};
        unsigned int m_buffer_object = 0;

        // member data to function
        unsigned int m_updatebuf_index = 0;
        unsigned int m_readbuf_index = 0;
        unsigned int m_previous_updatebuf_index = 0;

        // Alignment
        inline static [[nodiscard]] auto calculate_alignment_per_buffer(const unsigned int num_elements) -> size_t;

        // init functions
        void init_sync_type(const SpecialSyncType specialSyncType);
        void init_binding_data();
        void update_readbuf_binding_data();

        // flags
        constexpr auto get_storage_flags() const -> const GLbitfield;
        constexpr auto get_map_flags() const -> const GLbitfield;
        constexpr auto get_mem_barrier_flags() const -> const GLbitfield;

        // data
        inline void apply_mem_barrier_if_needed();
        void        set_persistent_map_data(CourierBuffer<DATA_TYPE>& buffer, const unsigned int index);
};

} // namespace vengine

#include "MutableBuffer.ipp"

#undef MUTABLE_BUFFER_IDENTIFIER
#undef MUTABLE_BUFFER_TEMPLATE
