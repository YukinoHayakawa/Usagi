#pragma once

#include <filesystem>

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
class FilesystemEventHandler
{
public:
    virtual ~FilesystemEventHandler() = default;

    virtual void file_added(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void file_removed(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void file_changed(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void file_renamed(
        std::uint64_t file_id,
        const std::filesystem::path &old_path,
        const std::filesystem::path &new_path) { }

    virtual void folder_added(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void folder_removed(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void folder_changed(
        std::uint64_t file_id,
        const std::filesystem::path &path) { }

    virtual void folder_renamed(
        std::uint64_t file_id,
        const std::filesystem::path &old_path,
        const std::filesystem::path &new_path) { }

    // Notify the user that the watcher has lost track of the changes happened
    // between the previous polling and the current one.
    virtual void out_of_sync() { }
};

class FilesystemWatcher : Noncopyable
{
protected:
    std::filesystem::path mBaseFolder;

public:
    explicit FilesystemWatcher(std::filesystem::path base_folder)
        : mBaseFolder(std::move(base_folder))
    {
    }

    virtual ~FilesystemWatcher() = default;

    virtual void poll_changes(FilesystemEventHandler &handler) = 0;

    const std::filesystem::path & base_folder() const
    {
        return mBaseFolder;
    }
};

std::unique_ptr<FilesystemWatcher> create_filesystem_watcher(
    std::filesystem::path base_folder
);
}
