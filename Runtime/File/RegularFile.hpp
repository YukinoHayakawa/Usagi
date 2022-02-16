#pragma once

#include <Usagi/Runtime/Platform/File.hpp>

#include "MappedFileView.hpp"

namespace usagi
{
class RegularFile
{
    std::filesystem::path mFilePath;
    platform::file::NativeFileHandle mFileHandle = USAGI_INVALID_FILE_HANDLE;
    platform::file::FileOpenMode mMode;

    void reset_handle();

public:
    RegularFile() = default;
    explicit RegularFile(
        std::filesystem::path path,
        platform::file::FileOpenMode mode = platform::file::FileOpenMode(
            platform::file::OPEN_READ | platform::file::OPEN_WRITE
        ),
        platform::file::FileOpenOptions options = { });
    ~RegularFile();

    RegularFile(const RegularFile &other) = delete;
    RegularFile(RegularFile &&other) noexcept;
    RegularFile & operator=(const RegularFile &other) = delete;
    RegularFile & operator=(RegularFile &&other) noexcept;

    /*
     * File Handle Info
     */

    std::filesystem::path path() const { return mFilePath; }
    platform::file::NativeFileHandle handle() const { return mFileHandle; }
    platform::file::FileOpenMode mode() const { return mMode; }
    std::size_t size() const;
    // File ID on filesystem
    std::uint64_t id() const;
    std::uint64_t last_modification_time() const;

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
        std::uint64_t offset = 0,
        std::uint64_t length = MappedFileView::USE_FILE_SIZE,
        std::uint64_t commit = 0,
        void *base_address = nullptr);
};
}
