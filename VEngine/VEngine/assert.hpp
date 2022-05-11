// This enables VE_ASSERT
#define VE_ENABLE_ASSERT

#ifdef VE_ENABLE_ASSERT

#ifndef VE_ENABLE_LOG_ERROR
#define VE_ENABLE_LOG_ERROR
#endif

#include "Logger.hpp"
#include <iostream>

#define VE_ASSERT(x)                                                                                                                       \
    if (!x)                                                                                                                                \
    {                                                                                                                                      \
        std::abort();                                                                                                                      \
    }

#define VE_ASSERT(x, y)                                                                                                                    \
    if (!x)                                                                                                                                \
    {                                                                                                                                      \
        VE_LOG_ERROR_MULTI_START("assert: " << y);                                                                                         \
        VE_LOG_MULTI("at: " << __FILE__);                                                                                                  \
        VE_LOG_ERROR_MULTI(" line:" << __LINE__);                                                                                          \
        VE_LOG_ERROR_MULTI(" inside:" << __FUNCTION__);                                                                                    \
        VE_LOG_ERROR_MULTI_END();                                                                                                          \
        std::abort();                                                                                                                      \
    }

#else
#define VE_ASSERT(x, y)
#define VE_ASSERT(x)
#endif
