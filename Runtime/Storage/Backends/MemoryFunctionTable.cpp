#include "MemoryFunctionTable.hpp"

namespace usagi::runtime::storage
{
const MemoryFunctionTable *standard_memory_functions() noexcept
{
    static constexpr MemoryFunctionTable table = {
        .map_view       = platforms::memory::map_file_view,
        .unmap_view     = platforms::memory::unmap_file_view,
        .remap_view     = platforms::memory::remap_view,
        .commit_pages   = platforms::memory::commit_pages,
        .decommit_pages = platforms::memory::decommit_pages,
        .lock_pages     = platforms::memory::lock_pages,
        .unlock_pages   = platforms::memory::unlock_pages,
        .zero_pages     = platforms::memory::zero_pages,
        .is_resident    = platforms::memory::is_resident,
        .prefetch       = platforms::memory::prefetch,
        .offer          = platforms::memory::offer,
        .flush          = platforms::memory::flush,
    };
    return &table;
}
} // namespace usagi::runtime::storage
