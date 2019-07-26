#include "File.hpp"

#include <fstream>

#include <Usagi/Core/Logging.hpp>

#include "Stream.hpp"

namespace fs = std::filesystem;

std::string usagi::readFileAsString(const fs::path &path)
{
    std::ifstream file(path, std::ios::binary);
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    const auto size = file_size(path);
    // data.resize(size);
    // file.read(data.data(), size);
    auto data = readStreamAsString(file);
    assert(data.size() == size);
    return data;
}

void usagi::dumpBinary(
    const std::filesystem::path &path,
    void *data,
    const std::size_t size)
{
    std::ofstream out(path, std::ios::binary);
    try
    {
        out.exceptions(std::ios::badbit | std::ios::failbit);
        out.write(static_cast<char*>(data), size);
    }
    catch(const std::exception &e)
    {
        LOG(error, "Could not write file to {}: {}, {}",
            path, e.what(), strerror(errno));
    }
    catch(...)
    {
        LOG(error, "Unknown error occurred while writing to {}.", path);
    }
}
