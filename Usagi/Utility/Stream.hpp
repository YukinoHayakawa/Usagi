#pragma once

#include <istream>
#include <string>
#include <vector>

namespace usagi
{
/**
 * \brief
 * \param in A seekable stream.
 * \return
 */
std::size_t streamLength(std::istream &in);

std::string readStreamToString(std::istream &in);
std::vector<std::uint8_t> readStreamToByteVector(std::istream &in);
}
