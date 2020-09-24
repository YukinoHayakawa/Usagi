#pragma once

#include <Usagi/Runtime/Platform/File.hpp>

#include "MappedFileView.hpp"

namespace usagi
{
class RegularFile
{
    std::u8string mUtf8Path;
    platform::file::NativeFileHandle mFileHandle = USAGI_INVALID_FILE_HANDLE;
    platform::file::FileOpenMode mMode;

    void reset_handle();

public:
    RegularFile(
        std::u8string path,
        platform::file::FileOpenMode mode,
        platform::file::FileOpenOptions options = { });
    ~RegularFile();

    RegularFile(const RegularFile &other) = delete;
    RegularFile(RegularFile &&other) noexcept;
    RegularFile & operator=(const RegularFile &other) = delete;
    RegularFile & operator=(RegularFile &&other) noexcept;

    /*
     * File Handle Info
     */

    std::u8string path() const { return mUtf8Path; }
    platform::file::NativeFileHandle handle() const { return mFileHandle; }
    platform::file::FileOpenMode mode() const { return mMode; }
    std::size_t size() const;

    /*
     * File Mapping
     */

    /**
     * \brief Create a memory mapping of the file. This file instance can
     * be discarded once the file view is created. In common operating systems,
     * the mapping internally holds a reference to the open file. If the
     * mapped length is larger than the file size, the file *may* be extended.
     * Accessing that part of the file guarantees extending the file length.
     * The access to the memory mapping inherits from the access to the file
     * of this object.
     * \param offset
     * \param length
     * \param commit
     * \param base_address Hint for base address of allocated virtual addresses.
     * The returned
     * \return
     */
    MappedFileView create_view(
        std::uint64_t offset,
        std::uint64_t length,
        std::uint64_t commit,
        void *base_address = nullptr);
};
}
