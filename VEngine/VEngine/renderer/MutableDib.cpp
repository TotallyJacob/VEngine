#include "MutableDib.h"

using namespace vengine;

MutableDib::MutableDib(unsigned int num_elements, bool gen)
    : MutableBuffer<IndirectElements, GL_DRAW_INDIRECT_BUFFER, 1>(num_elements, gen, SpecialSyncType::NORMAL_SYNC)
{
}
