#include "VirtualPageManager.hpp"

#include <algorithm>
#include <array>

#include "PageCommitStrategies.hpp"

namespace usagi::runtime::storage
{
using namespace details;

VirtualPageManager::VirtualPageManager(MemoryView bookkeeping_view) noexcept
    : mBookkeeping(std::move(bookkeeping_view))
{
    // Ensure the bookkeeping view is at least large enough for the header
    mBookkeeping.commit(0, sizeof(PageManagerHeader), CommitStrategy::Bypass);
    if(auto *h = header(); h->magic != 0x5650'4D47'5200'0000ull) // "VPMGR"
    {
        h->magic             = 0x5650'4D47'5200'0000ull;
        h->first_node_offset = 0;
        h->node_count        = 0;
        h->watermark_page    = 0;
    }
}

PageBitmapNode *VirtualPageManager::get_node(const std::uint64_t offset)
{
    if(offset == 0) return nullptr;
    return mBookkeeping.cast_view<PageBitmapNode>(offset);
}

PageBitmapNode *VirtualPageManager::find_or_create_node(
    const std::uint64_t page_index)
{
    auto               *h = header();
    const std::uint64_t target_start =
        (page_index / PageBitmapNode::PAGES_PER_NODE) *
        PageBitmapNode::PAGES_PER_NODE;

    PageBitmapNode *prev        = nullptr;
    std::uint64_t   curr_offset = h->first_node_offset;

    while(curr_offset != 0)
    {
        PageBitmapNode *curr = get_node(curr_offset);
        if(curr->start_page_index == target_start)
        {
            return curr;
        }
        prev        = curr;
        curr_offset = curr->next_node_offset;
    }

    // Allocate a new node
    const std::uint64_t new_offset =
        sizeof(PageManagerHeader) + (h->node_count * sizeof(PageBitmapNode));
    mBookkeeping.commit(
        new_offset, sizeof(PageBitmapNode), CommitStrategy::Bypass);

    PageBitmapNode *new_node   = get_node(new_offset);
    new_node->next_node_offset = 0;
    new_node->start_page_index = target_start;
    for(unsigned long long &mask : new_node->masks)
        mask = 0;

    if(prev)
    {
        prev->next_node_offset = new_offset;
    }
    else
    {
        h->first_node_offset = new_offset;
    }

    h->node_count++;
    return new_node;
}

namespace
{
template <typename T>
constexpr auto make_commit_func()
{
    return &T::commit;
}

template <typename T>
constexpr auto make_decommit_func()
{
    return &T::decommit;
}

constexpr std::array COMMIT_FUNCS = {
    make_commit_func<PageCommitStrategyExact>(),
    make_commit_func<PageCommitStrategyAggressive64>(),
    make_commit_func<PageCommitStrategyWatermark>(),
    make_commit_func<PageCommitStrategyBypass>(),
};

constexpr std::array DECOMMIT_FUNCS = {
    make_decommit_func<PageCommitStrategyExact>(),
    make_decommit_func<PageCommitStrategyAggressive64>(),
    make_decommit_func<PageCommitStrategyWatermark>(),
    make_decommit_func<PageCommitStrategyBypass>(),
};
} // namespace

void VirtualPageManager::commit(
    MemoryView *target_view, const std::uint64_t offset, const std::size_t size,
    CommitStrategy strategy)
{
    if(size == 0) return;

    const std::uint64_t page_size =
        to_bytes(target_view->traits().min_page_size);
    const std::uint64_t start_page = offset / page_size;
    const std::uint64_t end_page = (offset + size + page_size - 1) / page_size;

    if(const auto strategy_idx = static_cast<std::size_t>(strategy);
        strategy_idx < COMMIT_FUNCS.size())
    {
        COMMIT_FUNCS[strategy_idx](
            target_view, start_page, end_page, page_size, this);
    }
}

void VirtualPageManager::decommit(
    MemoryView *target_view, const std::uint64_t offset, const std::size_t size,
    CommitStrategy strategy)
{
    if(size == 0) return;

    const std::uint64_t page_size =
        to_bytes(target_view->traits().min_page_size);
    const std::uint64_t start_page = (offset + page_size - 1) / page_size;
    const std::uint64_t end_page   = (offset + size) / page_size;

    if(start_page >= end_page && strategy != CommitStrategy::Watermark) return;

    if(const auto strategy_idx = static_cast<std::size_t>(strategy);
        strategy_idx < DECOMMIT_FUNCS.size())
    {
        DECOMMIT_FUNCS[strategy_idx](
            target_view, start_page, end_page, page_size, this);
    }
}
} // namespace usagi::runtime::storage
