#pragma once

#include <istream>
#include <string>

namespace usagi
{
/**
 * \brief Pump all characters from the input stream into a string. Note that
 * the char stream may be altered unintentionally if the stream is not opened as
 * binary.
 * \param in
 * \return
 */
std::string readStreamAsString(std::istream &in);
}
