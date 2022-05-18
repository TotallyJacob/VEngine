namespace vengine
{
namespace util
{

/*
    Opengl functions
 */

TT inline static void buffer_storage(const GLenum target, unsigned int num_elements, const void* initialData, const unsigned int flags)
{
    glBufferStorage(target, sizeof(T) * num_elements, initialData, flags);
}

inline static void buffer_storage_with_alignment(const GLenum target, GLsizeiptr total_size_bytes, const void* initialData,
                                                 const unsigned int flags)
{
    glBufferStorage(target, total_size_bytes, initialData, flags);
}

[[nodiscard]] inline static char* map_buffer_range_with_alignment(const GLenum target, const GLintptr offset, const GLsizeiptr size,
                                                                  const unsigned int flags)
{
    return static_cast<char*>(glMapBufferRange(target, offset, size, flags));
}

TT [[nodiscard]] inline static T* map_buffer_range_index(const GLenum target, unsigned int num_elements, unsigned int startingIndex,
                                                         const unsigned int flags)
{
    return (T*)glMapBufferRange(target, startingIndex * sizeof(T), num_elements * sizeof(T), flags);
}

TT [[nodiscard]] inline static T* map_buffer_range(const GLenum target, unsigned int num_elements, const unsigned int flags)
{
    return (T*)glMapBufferRange(target, 0, sizeof(T) * num_elements, flags);
}


/*
 * MEMORY FUNCTIONS
 */


DT inline static void insert_data(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                  unsigned int numElementsToShift)
{
    util::shift_data(initial, indexToInsertAt, indexToInsertAt + toInsert.size(), numElementsToShift);
    util::buf_memcpy(initial, &toInsert.front(), toInsert.size(), indexToInsertAt);
}

DT inline static void insert_data_no_shift(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt)
{

    util::buf_memcpy(initial, &toInsert.front(), toInsert.size(), indexToInsertAt);
}

DT inline static void insert_all_data(DATA_TYPE* initial, std::vector<DATA_TYPE>& toInsert, unsigned int indexToInsertAt,
                                      unsigned int num_elements_to_insert)
{
    util::insert_data(initial, toInsert, indexToInsertAt, num_elements_to_insert - indexToInsertAt);
}


DT inline static void shift_data(DATA_TYPE* initial, unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                 unsigned int numElementsToShift)
{
    util::buf_memcpy(initial, &initial[indexToShiftFrom], numElementsToShift, indexToShiftTo);
}

DT inline static void shift_data_virtual_max(DATA_TYPE* initial, unsigned int indexToShiftFrom, unsigned int indexToShiftTo,
                                             unsigned int virtualMaxNumberOfElements)
{
    util::shift_data(initial, indexToShiftFrom, indexToShiftTo, virtualMaxNumberOfElements - indexToShiftFrom);
}

/*
    ACTUAL MEMCOPY STUFF
*/

DT inline static void buf_fill(DATA_TYPE* persistent_map, const DATA_TYPE* data, unsigned int num_elements_in_array)
{
    std::fill_n(persistent_map, num_elements_in_array, *data);
}

DT inline static void buf_memcpy(DATA_TYPE* persistentMap, const DATA_TYPE* data, unsigned int numDataToCpy,
                                 unsigned int persistentMapStartPoint)
{
    std::memcpy(&persistentMap[persistentMapStartPoint], data, numDataToCpy * sizeof(DATA_TYPE));
}
} // namespace util
} // namespace vengine