#pragma once

#include <string>
#include <filesystem>

namespace usagi
{
/**
 * \brief Read the full sequence of bytes into a string from a file, which
 * is treated as binary.
 * \param path
 * \return
 */
std::string readFileAsString(const std::filesystem::path &path);

void dumpBinary(
    const std::filesystem::path &path,
    void *data,
    std::size_t size);
}
