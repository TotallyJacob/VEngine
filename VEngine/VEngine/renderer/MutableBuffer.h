#pragma once

#include "GL/glew.h"

#include "../ByteArray.h"
#include "../Logger.hpp"
#include "../Timer.h"
#include "IMutableBuffer.h"
#include "PersistentMapBufferUtil.h"

#include <assert.h>
#include <cmath>
#include <iostream>
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

#define MUTABLE_BUFFER_IDENTIFIER <DATA_TYPE, buffer_type, num_buffers, thread_safe>
#define MUTABLE_BUFFER_TEMPLATE                                                                                                            \
    template <typename DATA_TYPE, const GLenum buffer_type, const unsigned int num_buffers, const bool thread_safe = false>

MUTABLE_BUFFER_TEMPLATE
class MutableBuffer : public IMutableBuffer
{

    public:

        MutableBuffer()
        {
        }
        MutableBuffer(const unsigned int num_elements, const bool gen = false,
                      const SpecialSyncType specialSyncType = SpecialSyncType::NORMAL_SYNC);

        // Init stuff
        void gen(unsigned int num_elements, const SpecialSyncType specialSyncType = SpecialSyncType::NORMAL_SYNC);
        void init_sync_type(const SpecialSyncType specialSyncType);

        // Binding stuff
        template <bool delete_readbuf_sync>
        void        swap_buffers();
        inline void delete_buffers();
        inline void bind() const;
        inline void unbind() const;

        // Syncs

        template <bool print_if_sync_fail, bool print_sync_time>
        void standard_wait_sync(const GLsync& sync, const std::string& print_sync_message = "", const GLuint64 wait_time = 100000);

        inline void        delete_fence_syncs();
        inline bool        readbuf_sync_invalid();
        inline bool        updatebuf_sync_invalid();
        inline void        delete_readbuf_sync();
        inline void        insert_sync_on_readbuf();
        inline static bool sync_invalid(const auto& sync);


        // useful data functions
        void        set_initial_data(unsigned int indexToChange, DATA_TYPE valueToChangeTo);
        void        set_initial_data(DATA_TYPE* value);
        void        set_initial_data(DATA_TYPE* values, unsigned int numData);
        inline void copy_previous_updatebuf_into_updatebuf();


        // insert
        void insert_data(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt, unsigned int numElementsToShift);
        void insert_data_no_shift(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt);
        void insert_data(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt);
        void insert_data_virtual_max(std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                     unsigned int virtualMaxNumberOfElements);
        void shift_data(unsigned int indexToShiftFrom, unsigned int indexToShiftTo, unsigned int numElementsToShift);
        void shift_data_virtual_max(unsigned int indexToShiftFrom, unsigned int indexToShiftTo, unsigned int virtualMaxNumberOfElements);
        void set_updatebuf_data(DATA_TYPE* data, unsigned int numOfElements, unsigned int startPoint);
        void fill_persistent_map(DATA_TYPE* value_to_fill);

        // Actual mem copy stuff
        void set_persistent_map_data(const unsigned int index, DATA_TYPE* data, unsigned int numOfElements, unsigned int startPoint);
        void fill_persistent_map(const unsigned int index, DATA_TYPE* value_to_fill);

        // Getters
        auto get_readbuf_sync() -> GLsync&;
        auto get_updatebuf_sync() -> GLsync&;
        auto get_persistent_map(const unsigned int index) -> DATA_TYPE*;
        auto get_updatebuf_persistent_map(const unsigned int index) -> DATA_TYPE*;
        auto get_byte_array() -> ByteArray<DATA_TYPE, num_buffers>&;

        auto           get_alignment_per_buffer() const -> const size_t;
        auto           get_sizeof_elements_per_buf() const -> const size_t;
        constexpr auto get_buffer_type() const -> const GLenum;
        auto           get_buffer_object() const -> const unsigned int;
        constexpr auto get_num_buffers() const -> const unsigned int;
        auto           get_num_elements() const -> const unsigned int;

    protected:

        constexpr const static GLsync       m_invalid_sync_object = 0;
        constexpr const static unsigned int m_persistent_map_flags = util::persistent_map_flags;

        // gl arrays
        ByteArray<DATA_TYPE, num_buffers> m_persistent_maps = {};
        GLsync                            m_fences[num_buffers] = {0};
        unsigned int                      m_buffer_object = 0;

        // member data to function
        unsigned int m_updatebuf_index = 0;
        unsigned int m_readbuf_index = 0;
        unsigned int m_previous_updatebuf_index = 0;

        // Alignment
        inline static [[nodiscard]] auto calculate_alignment_per_buffer(const unsigned int num_elements) -> size_t;
};

} // namespace vengine

#include "MutableBuffer.ipp"

#undef MUTABLE_BUFFER_IDENTIFIER
#undef MUTABLE_BUFFER_TEMPLATE
