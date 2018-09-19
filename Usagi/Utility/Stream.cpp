#include "Stream.hpp"

std::string usagi::readStreamAsString(std::istream &in)
{
    // allows using with forward only streams
    const std::istreambuf_iterator<char> begin(in), end;
    return { begin, end };
}
