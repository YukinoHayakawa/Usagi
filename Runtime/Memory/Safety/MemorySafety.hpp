#pragma once

#include <expected>

#include "MemoryAccessErrorCodes.hpp"

namespace usagi::runtime::memory
{
/*
 * Shio: The function now returns std::expected to provide detailed error
 * information, as you suggested. On success, it contains 'true'. On failure,
 * it contains a MemoryAccessErrorCodes value.
 */
std::expected<void, MemoryAccessErrorCodes> is_address_readable(void * ptr);
} // namespace usagi::runtime::memory
