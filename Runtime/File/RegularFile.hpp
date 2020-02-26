#pragma once

#include <Usagi/Runtime/Platform/File.hpp>

namespace usagi
{
class RegularFile
{
    std::u8string mUtf8Path;
    platform::file::NativeFileHandle mFileHandle { };
    platform::file::FileOpenMode mMode;

public:
    RegularFile() = default;
    RegularFile(
        std::u8string path,
        platform::file::FileOpenMode mode,
        platform::file::FileOpenOptions options = { });
    ~RegularFile();

    RegularFile(RegularFile &&other) noexcept;
    RegularFile & operator=(RegularFile &&other) noexcept;

    std::u8string path() const { return mUtf8Path; }
    platform::file::NativeFileHandle handle() const { return mFileHandle; }
    platform::file::FileOpenMode mode() const { return mMode; }
    std::size_t size() const;

    std::size_t read(std::size_t offset, void *buf, std::size_t size);
    std::size_t write(std::size_t offset, const void *buf, std::size_t size);
};
}
