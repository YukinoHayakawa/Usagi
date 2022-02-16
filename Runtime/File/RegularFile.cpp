#include "RegularFile.hpp"

#include <cassert>

#include "MappedFileView.hpp"

namespace usagi
{
void RegularFile::reset_handle()
{
    mFileHandle = USAGI_INVALID_FILE_HANDLE;
}

RegularFile::RegularFile(
    std::filesystem::path path,
    const platform::file::FileOpenMode mode,
    const platform::file::FileOpenOptions options)
    : mFilePath(std::move(path))
    , mMode(mode)
{
    using namespace platform::file;
    mFileHandle = open(mFilePath, mode, options);
    assert(mFileHandle != USAGI_INVALID_FILE_HANDLE);
}

RegularFile::~RegularFile()
{
    if(mFileHandle) platform::file::close(mFileHandle);
}

RegularFile::RegularFile(RegularFile &&other) noexcept
    : mFilePath { std::move(other.mFilePath) }
    , mFileHandle { other.mFileHandle }
    , mMode { other.mMode }
{
    // resetting the file handle in the source instance so it doesn't get
    // double released.
    other.reset_handle();
}

RegularFile & RegularFile::operator=(RegularFile &&other) noexcept
{
    if(this == &other)
        return *this;
    mFilePath = std::move(other.mFilePath);
    mFileHandle = other.mFileHandle;
    mMode = other.mMode;
    other.reset_handle();
    return *this;
}

std::size_t RegularFile::size() const
{
    return platform::file::size(mFileHandle);
}

std::uint64_t RegularFile::id() const
{
    return platform::file::id(mFileHandle);
}

std::uint64_t RegularFile::last_modification_time() const
{
    return platform::file::last_modification_time(mFileHandle);
}

MappedFileView RegularFile::create_view(
    std::uint64_t offset,
    std::uint64_t length,
    std::uint64_t commit,
    void *base_address)
{
    using namespace platform::file;

    int mode = 0;

    if(mMode & OPEN_READ) mode |= MAPPING_READ;
    if(mMode & OPEN_WRITE) mode |= MAPPING_WRITE;

    return {
        this,
        static_cast<MemoryMappingMode>(mode),
        offset,
        length,
        commit
    };
}
}
