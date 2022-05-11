#pragma once

#include "GL/glew.h"

#include <iostream>

namespace vengine::util
{

constexpr static unsigned int persistent_map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

template <typename T>
inline static void buffer_storage(const GLenum target, unsigned int num_elements, const void* initialData = nullptr,
                                  const unsigned int flags = persistent_map_flags)
{
    glBufferStorage(target, sizeof(T) * num_elements, initialData, flags);
}

inline static void buffer_storage_with_alignment(const GLenum target, GLsizeiptr total_size_bytes, const void* initialData = nullptr,
                                                 const unsigned int flags = persistent_map_flags)
{
    glBufferStorage(target, total_size_bytes, initialData, flags);
}

[[nodiscard]] inline static char* map_buffer_range_with_alignment(const GLenum target, const GLintptr offset, const GLsizeiptr size,
                                                                  const unsigned int flags = persistent_map_flags)
{
    return static_cast<char*>(glMapBufferRange(target, offset, size, flags));
}

template <typename T>
[[nodiscard]] inline static T* map_buffer_range_index(const GLenum target, unsigned int num_elements, unsigned int startingIndex,
                                                      const unsigned int flags = persistent_map_flags)
{
    return (T*)glMapBufferRange(target, startingIndex * sizeof(T), num_elements * sizeof(T), flags);
}

template <typename T>
[[nodiscard]] inline static T* map_buffer_range(const GLenum target, unsigned int num_elements,
                                                const unsigned int flags = persistent_map_flags)
{
    return (T*)glMapBufferRange(target, 0, sizeof(T) * num_elements, flags);
}

template <typename ARRAY_TYPE>
inline static void persistent_map_fill(ARRAY_TYPE* persistent_map, ARRAY_TYPE* data, unsigned int num_elements_in_array)
{
    std::fill_n(persistent_map, num_elements_in_array, *data);
}

template <typename ARRAY_TYPE>
inline static void persistent_map_memcpy(ARRAY_TYPE* persistentMap, ARRAY_TYPE* data, unsigned int numDataToCpy,
                                         unsigned int persistentMapStartPoint)
{
    std::memcpy(&persistentMap[persistentMapStartPoint], data, numDataToCpy * sizeof(ARRAY_TYPE));
}

} // namespace vengine::util
