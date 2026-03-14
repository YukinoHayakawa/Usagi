#pragma once

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

namespace usagi::runtime::storage
{
namespace details
{
struct PageManagerHeader
{
    std::uint64_t magic;
    std::uint64_t first_node_offset;
    std::uint64_t node_count;
    std::uint64_t watermark_page;
};

struct PageBitmapNode
{
    using BitOps = platforms::instructions::
        DefaultBitManipulationInstructions<OperandBitWidth::_64>;
    using MaskType = BitOps::ValueType;

    static constexpr std::uint64_t BITS_PER_MASK =
        static_cast<std::uint64_t>(BitOps::BIT_WIDTH);
    static constexpr std::uint64_t MASKS_PER_NODE = 62;
    static constexpr std::uint64_t PAGES_PER_NODE =
        BITS_PER_MASK * MASKS_PER_NODE;

    std::uint64_t next_node_offset;
    std::uint64_t start_page_index;
    MaskType      masks[MASKS_PER_NODE];
};

static_assert(sizeof(PageBitmapNode) == 512);
} // namespace details

/**
 * Shio:
 * Mediates physical page commitment for MemoryViews.
 * This class uses its own internal MemoryView (backed by a fast
 * PagefileBackend) to store tracking bitmaps so that the user's primary
 * MemoryView has zero internal fragmentation.
 *
 * It is neutral to all storage backends and allocators.
 */
class VirtualPageManager
{
    // The backing buffer for bookkeeping data (typically a linearly growing
    // pagefile mapping)
    MemoryView mBookkeeping;

public:
    explicit VirtualPageManager(MemoryView bookkeeping_view) noexcept;

    /**
     * @brief Ensures that the specified logical range is backed by physical
     * pages.
     */
    void commit(
        MemoryView *target_view, std::uint64_t offset, std::size_t size,
        CommitStrategy strategy);

    /**
     * @brief Decommits the physical pages backing the specified logical range,
     * releasing them back to the OS.
     */
    void decommit(
        MemoryView *target_view, std::uint64_t offset, std::size_t size,
        CommitStrategy strategy);

    [[nodiscard]]
    details::PageBitmapNode *get_node(std::uint64_t offset);

    [[nodiscard]]
    details::PageBitmapNode *find_or_create_node(std::uint64_t page_index);

    [[nodiscard]]
    details::PageManagerHeader *header()
    {
        return mBookkeeping.cast_view<details::PageManagerHeader>();
    }
};
} // namespace usagi::runtime::storage
