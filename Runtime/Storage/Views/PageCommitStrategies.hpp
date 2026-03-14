#pragma once

#include <concepts>

#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Concept ensuring that a given type implements the required static methods
 * for a page commit strategy.
 */
template <typename T>
// todo: return expected
concept PageCommitStrategyImplementation = requires(
    MemoryView *target_view, std::uint64_t start_page, std::uint64_t end_page,
    std::uint64_t page_size, VirtualPageManager *vpm) {
    {
        T::commit(target_view, start_page, end_page, page_size, vpm)
    } -> std::same_as<void>;
    {
        T::decommit(target_view, start_page, end_page, page_size, vpm)
    } -> std::same_as<void>;
};

struct PageCommitStrategyExact
{
    static void commit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
    static void decommit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
};

struct PageCommitStrategyAggressive64
{
    static void commit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
    static void decommit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
};

struct PageCommitStrategyWatermark
{
    static void commit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
    static void decommit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
};

struct PageCommitStrategyBypass
{
    static void commit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
    static void decommit(
        MemoryView *target_view, std::uint64_t start_page,
        std::uint64_t end_page, std::uint64_t page_size,
        VirtualPageManager *vpm);
};

static_assert(PageCommitStrategyImplementation<PageCommitStrategyExact>);
static_assert(PageCommitStrategyImplementation<PageCommitStrategyAggressive64>);
static_assert(PageCommitStrategyImplementation<PageCommitStrategyWatermark>);
static_assert(PageCommitStrategyImplementation<PageCommitStrategyBypass>);
} // namespace usagi::runtime::storage
