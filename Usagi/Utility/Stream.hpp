#pragma once

#include <istream>
#include <string>
#include <vector>

namespace usagi
{
/**
 * \brief Pump all characters from the input stream into a string. Note that
 * the char stream may be altered unintentionally if the stream is not opened
 * as binary.
 * \tparam ContainerT
 * \param in
 * \return
 */
template <typename ContainerT>
ContainerT readStreamToEnd(std::istream &in)
{
    // allows using with forward only streams
    const std::istreambuf_iterator<char> begin(in), end;
    return { begin, end };
}

inline auto readStreamToString(std::istream &in)
{
    return readStreamToEnd<std::string>(in);
}

inline auto readStreamToByteVector(std::istream &in)
{
    return readStreamToEnd<std::vector<std::uint8_t>>(in);
}
}
