#include "Stream.hpp"

#include <cassert>

std::size_t usagi::streamLength(std::istream &in)
{
    assert(in.tellg() == 0);
    in.seekg(0, in.end);
    const auto size = in.tellg();
    in.seekg(0, in.beg);
    return size;
}

std::string usagi::readStreamToString(std::istream &in)
{
    // https://en.cppreference.com/w/cpp/iterator/istreambuf_iterator
    // istreambuf_iterator is a input_iterator, which means building from it
    // is equivalent to use a while loop to read byte-by-byte, which is very
    // inefficient.
    // const std::istreambuf_iterator<char> begin(in), end;
    // return { begin, end };

    const auto size = streamLength(in);
    std::string buf;
    buf.resize(size);
    in.read(buf.data(), size);
    return buf;
}

std::vector<std::uint8_t> usagi::readStreamToByteVector(std::istream &in)
{
    static_assert(sizeof(std::uint8_t) == sizeof(char));

    const auto size = streamLength(in);
    std::vector<std::uint8_t> buf;
    buf.resize(size);
    in.read(reinterpret_cast<char*>(buf.data()), size);
    return buf;
}
