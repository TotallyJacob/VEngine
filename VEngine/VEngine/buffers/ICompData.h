#pragma once

#include "MutableBufferOptions.hpp"

namespace buf
{

class ICompData
{
    public:

        virtual ~ICompData() = default;
        virtual void destroy_id(Id id) = 0;
};

}; // namespace buf