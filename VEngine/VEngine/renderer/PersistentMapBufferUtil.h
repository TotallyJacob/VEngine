#pragma once

#include "GL/glew.h"

#include <iostream>
#include <vector>

namespace vengine::util
{

#define TT template <typename T>
#define DT template <typename DATA_TYPE>

constexpr static GLbitfield persistent_map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

// opengl functions
TT inline static void buffer_storage(const GLenum target, unsigned int num_elements, const void* initialData = nullptr,
                                     const unsigned int flags = persistent_map_flags);
inline static void    buffer_storage_with_alignment(const GLenum target, GLsizeiptr total_size_bytes, const void* initialData = nullptr,
                                                    const unsigned int flags = persistent_map_flags);
[[nodiscard]] inline static char* map_buffer_range_with_alignment(const GLenum target, const GLintptr offset, const GLsizeiptr size,
                                                                  const unsigned int flags = persistent_map_flags);
TT [[nodiscard]] inline static T* map_buffer_range_index(const GLenum target, unsigned int num_elements, unsigned int startingIndex,
                                                         const unsigned int flags = persistent_map_flags);

TT [[nodiscard]] inline static T* map_buffer_range(const GLenum target, unsigned int num_elements,
                                                   const unsigned int flags = persistent_map_flags);



// insert
DT inline static void insert_data(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                  unsigned int numElementsToShift);
DT inline static void insert_data_no_shift(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt);
DT inline static void insert_all_data(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                      unsigned int num_elements_to_insert);
// shift
DT inline static void shift_data(DATA_TYPE* initial, unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                 unsigned int numElementsToShift);
DT inline static void shift_data_virtual_max(DATA_TYPE* initial, unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                             unsigned int virtualMaxNumberOfElements);

// mem
DT inline static void buf_fill(DATA_TYPE* persistent_map, const DATA_TYPE* data, unsigned int num_elements_in_buf);
DT inline static void buf_memcpy(DATA_TYPE* persistentMap, const DATA_TYPE* data, unsigned int numDataToCpy, unsigned int startPoint);



} // namespace vengine::util

#include "PersistentMapBufferUtil.ipp"

#undef TT
#undef DT