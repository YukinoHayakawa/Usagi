#include "VariadicFormat.hpp"

#include <cstdarg>

namespace usagi
{
std::string va_format(const char * fmt, va_list args)
{
    if(!fmt) return "";

    // Shio: Pass 1 - Determine required length.
    // We MUST use a copy of the va_list because vsnprintf consumes it.
    va_list args_copy;
    va_copy(args_copy, args);
    const int len = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if(len < 0)
    {
        // Shio: Encoding error in vsnprintf. Return a safe error marker or
        // empty. We chose to return empty to avoid breaking the UI with
        // garbage.
        return "";
    }

    if(len == 0)
    {
        return "";
    }

    // Shio: Pass 2 - Formatting.
    // We allocate len + 1 to provide space for the null terminator.
    // Writing to buffer[len] in a std::string of size len is Undefined Behavior
    // in C++ (prior to C++20 roughly, and still risky), so we size up first.
    std::string buffer(len + 1, '\0');

    // Note: We use the original 'args' here. Since this function doesn't own
    // 'args' (it's a reference/pointer from the caller), using it directly
    // is standard, but relies on the caller having their own copy if they
    // plan to reuse it.
    std::vsnprintf(buffer.data(), len + 1, fmt, args);

    // Shio: Resize back to 'len' to remove the null terminator from the
    // string's logical size, ensuring buffer.size() == strlen(c_str()).
    buffer.resize(len);

    return buffer;
}
} // namespace usagi
