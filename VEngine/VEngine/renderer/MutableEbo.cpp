#include "MutableEbo.h"

using namespace vengine;

MutableEbo::MutableEbo(unsigned int num_elements, bool gen)
    : MutableBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, 1>(num_elements, gen, SpecialSyncType::NORMAL_SYNC)
{
}
