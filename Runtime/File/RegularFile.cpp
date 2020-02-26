#include "RegularFile.hpp"

namespace usagi
{
RegularFile::RegularFile(
    std::u8string path,
    const platform::file::FileOpenMode mode,
    const platform::file::FileOpenOptions options)
    : mUtf8Path(std::move(path))
    , mMode(mode)
{
    mFileHandle = platform::file::open(mUtf8Path, mode, options);
}

RegularFile::~RegularFile()
{
    if(mFileHandle)
    {
        platform::file::close(mFileHandle);
    }
}

RegularFile::RegularFile(RegularFile &&other) noexcept
    : mUtf8Path { std::move(other.mUtf8Path) }
    , mFileHandle { other.mFileHandle }
    , mMode { other.mMode }
{
    other.mFileHandle = decltype(mFileHandle) { };
}

RegularFile & RegularFile::operator=(RegularFile &&other) noexcept
{
    if(this == &other)
        return *this;

    mUtf8Path = std::move(other.mUtf8Path);
    mFileHandle = other.mFileHandle;
    mMode = other.mMode;

    other.mFileHandle = decltype(mFileHandle) { };

    return *this;
}

std::size_t RegularFile::size() const
{
    return platform::file::size(mFileHandle);
}

std::size_t RegularFile::read(
    const std::size_t offset,
    void *buf,
    const std::size_t size)
{
    return platform::file::read_at(mFileHandle, offset, buf, size);
}

std::size_t RegularFile::write(
    const std::size_t offset,
    const void *buf,
    const std::size_t size)
{
    return platform::file::write_at(mFileHandle, offset, buf, size);
}
}
