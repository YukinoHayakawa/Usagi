#include "PageCommitStrategies.hpp"

#include <Usagi/Runtime/Storage/Views/VirtualPageManager.hpp>

namespace usagi::runtime::storage
{
using namespace details;

void PageCommitStrategyExact::commit(
    MemoryView *target_view, const std::uint64_t start_page,
    const std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    using MaskType = PageBitmapNode::MaskType;
    for(std::uint64_t p = start_page; p < end_page; ++p)
    {
        PageBitmapNode     *node     = vpm->find_or_create_node(p);
        const std::uint64_t local_p  = p - node->start_page_index;
        const std::uint64_t mask_idx = local_p / PageBitmapNode::BITS_PER_MASK;
        const std::uint64_t bit_idx  = local_p % PageBitmapNode::BITS_PER_MASK;

        if((node->masks[mask_idx] & (static_cast<MaskType>(1) << bit_idx)) == 0)
        {
            target_view->commit(
                p * page_size, page_size, CommitStrategy::Bypass);
            node->masks[mask_idx] |= (static_cast<MaskType>(1) << bit_idx);
        }
    }
}

void PageCommitStrategyExact::decommit(
    MemoryView *target_view, const std::uint64_t start_page,
    const std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    using MaskType = PageBitmapNode::MaskType;
    for(std::uint64_t p = start_page; p < end_page; ++p)
    {
        PageBitmapNode     *node     = vpm->find_or_create_node(p);
        const std::uint64_t local_p  = p - node->start_page_index;
        const std::uint64_t mask_idx = local_p / PageBitmapNode::BITS_PER_MASK;
        const std::uint64_t bit_idx  = local_p % PageBitmapNode::BITS_PER_MASK;

        if((node->masks[mask_idx] & (static_cast<MaskType>(1) << bit_idx)) != 0)
        {
            target_view->decommit(
                p * page_size, page_size, CommitStrategy::Bypass);
            node->masks[mask_idx] &= ~(static_cast<MaskType>(1) << bit_idx);
        }
    }
}

void PageCommitStrategyAggressive64::commit(
    MemoryView *target_view, const std::uint64_t start_page,
    const std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    using MaskType = PageBitmapNode::MaskType;
    const std::uint64_t start_mask_aligned =
        start_page / PageBitmapNode::BITS_PER_MASK;
    const std::uint64_t end_mask_aligned =
        (end_page + PageBitmapNode::BITS_PER_MASK - 1) /
        PageBitmapNode::BITS_PER_MASK;

    for(std::uint64_t m = start_mask_aligned; m < end_mask_aligned; ++m)
    {
        const std::uint64_t base_page = m * PageBitmapNode::BITS_PER_MASK;
        PageBitmapNode     *node      = vpm->find_or_create_node(base_page);
        const std::uint64_t local_p   = base_page - node->start_page_index;
        const std::uint64_t mask_idx  = local_p / PageBitmapNode::BITS_PER_MASK;

        if(node->masks[mask_idx] != static_cast<MaskType>(~0ull))
        {
            target_view->commit(
                base_page * page_size,
                PageBitmapNode::BITS_PER_MASK * page_size,
                CommitStrategy::Bypass);
            node->masks[mask_idx] = static_cast<MaskType>(~0ull);
        }
    }
}

void PageCommitStrategyAggressive64::decommit(
    MemoryView *target_view, const std::uint64_t start_page,
    const std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    PageCommitStrategyExact::decommit(
        target_view, start_page, end_page, page_size, vpm);
}

void PageCommitStrategyWatermark::commit(
    MemoryView *target_view, [[maybe_unused]] std::uint64_t start_page,
    const std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    if(PageManagerHeader *header = vpm->header();
        end_page > header->watermark_page)
    {
        const std::uint64_t pages_to_commit = end_page - header->watermark_page;
        target_view->commit(
            header->watermark_page * page_size,
            pages_to_commit * page_size,
            CommitStrategy::Bypass);
        header->watermark_page = end_page;
    }
}

void PageCommitStrategyWatermark::decommit(
    MemoryView *target_view, [[maybe_unused]] std::uint64_t start_page,
    [[maybe_unused]] std::uint64_t end_page, const std::uint64_t page_size,
    VirtualPageManager *vpm)
{
    if(PageManagerHeader *header = vpm->header(); header->watermark_page > 0)
    {
        target_view->decommit(
            0, header->watermark_page * page_size, CommitStrategy::Bypass);
        header->watermark_page = 0;
    }
}

void PageCommitStrategyBypass::commit(
    [[maybe_unused]] MemoryView         *target_view,
    [[maybe_unused]] std::uint64_t       start_page,
    [[maybe_unused]] std::uint64_t       end_page,
    [[maybe_unused]] std::uint64_t       page_size,
    [[maybe_unused]] VirtualPageManager *vpm)
{
    // Do nothing. Managed by memory view internally.
}

void PageCommitStrategyBypass::decommit(
    [[maybe_unused]] MemoryView         *target_view,
    [[maybe_unused]] std::uint64_t       start_page,
    [[maybe_unused]] std::uint64_t       end_page,
    [[maybe_unused]] std::uint64_t       page_size,
    [[maybe_unused]] VirtualPageManager *vpm)
{
    // Do nothing. Managed by memory view internally.
}
} // namespace usagi::runtime::storage
