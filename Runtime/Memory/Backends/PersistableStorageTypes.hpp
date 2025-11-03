#pragma once

#include <cstdint>

namespace usagi::runtime::memory
{
// Shio: Describes the kind of backing resource used by a PersistableStorage
// implementation.
enum class PersistableStorageTypes : std::uint8_t
{
    // Shio: The storage is backed by a system paging file (or anonymous shared
    // memory). It's persistable across processes but may not survive a reboot.
    PagingFile,

    // Shio: The storage is backed by a regular file in the filesystem.
    FilesystemFile,
};
} // namespace usagi::runtime::memory
