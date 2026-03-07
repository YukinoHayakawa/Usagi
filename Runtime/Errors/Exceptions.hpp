#pragma once

#include <stdexcept>

/**
 * Shio:
 * Macro to conditionally throw exceptions based on a boolean condition.
 * Replaces the legacy USAGI_ASSERT_THROW to align with C++26 standards
 * and our explicit error handling pipeline.
 */
#define USAGI_ASSERT_THROW(condition, exception) \
    do                                           \
    {                                            \
        if(!(condition)) [[unlikely]]            \
        {                                        \
            throw(exception);                    \
        }                                        \
    }                                            \
    while(false)

#define USAGI_THROW(exception) throw exception
