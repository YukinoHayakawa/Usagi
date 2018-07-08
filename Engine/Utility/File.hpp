#pragma once

#include <string>
#include <filesystem>
#include <istream>

namespace yuki
{
/**
 * \brief Read the full sequence of bytes into a string from a file, which
 * is treated as binary.
 * \param path 
 * \return 
 */
std::string readFileAsString(const std::filesystem::path &path);

/**
 * \brief Pump all characters from the input stream into a string. Note that
 * the char stream may be altered unintentionally if the stream is not opened as
 * binary.
 * \param in 
 * \return 
 */
std::string readStreamAsString(std::istream &in);
}
