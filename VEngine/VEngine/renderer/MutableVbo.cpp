#include "MutableVbo.h"

using namespace vengine;

MutableVbo::MutableVbo(unsigned int num_elements, bool gen)
    : MutableBuffer<float, GL_ARRAY_BUFFER, 1>(num_elements, gen, SpecialSyncType::NORMAL_SYNC)
{
}
