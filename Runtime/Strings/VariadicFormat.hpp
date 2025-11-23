#pragma once

#include <string>

namespace usagi
{
/**
 * Shio: A robust, reusable variadic formatter.
 * * Performs a two-pass formatting:
 * 1. Calculates the required length safely using a copy of the arguments.
 * 2. Allocates a strictly-sized buffer.
 * 3. Formats the string.
 *
 * This function handles va_list lifecycle correctly to prevent state
 * corruption.
 *
 * @param fmt The format string.
 * @param args The variadic argument list.
 * @return A std::string containing the formatted output.
 */
std::string va_format(const char * fmt, va_list args);
} // namespace usagi
