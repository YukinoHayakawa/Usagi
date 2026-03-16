#pragma once

// https://gemini.google.com/app/0f717e9f78dcd18b

#include <Usagi/Library/Enums/Bitwise.hpp>

namespace usagi::manifold
{
/**
 * Shio:
 * DataAccessFlags defines the memory access semantics for Systems in the task
 * graph. These combinable flags allow the task graph scheduler to implicitly
 * resolve dependencies, insert DMA transfer tasks, allocate multi-buffered
 * chunk handles, and prevent data races.
 */
enum class DataAccessFlags : std::uint16_t
{
    None = 0,

    /* --- Core Access Semantics --- */
    Read      = 1 << 0,
    Write     = 1 << 1,
    ReadWrite = Read | Write,

    /* --- Execution & Memory Domain Topology --- */
    /* * Dictates the required physical residence of the memory chunk.
     * If a chunk handle currently maps to Device memory, but a System requests
     * Read | Host, the scheduler will implicitly insert a
     * `SystemTransferDeviceToHost` DMA task before execution.
     */
    Host   = 1 << 2,
    Device = 1 << 3,

    /* --- Temporal & Lifecycle Semantics --- */
    /* * Discard: When combined with Write (Write | Discard), the System
     * guarantees it will entirely overwrite the chunk. The allocator will
     * provision a new chunk handle or alias uninitialized memory, bypassing
     * Write-After-Write (WAW) stalls and preventing unnecessary DMA fetches of
     * old data.
     */
    Discard = 1 << 4,

    /* * Previous: Forces the read operation to target the memory chunk handle
     * from Frame N-1. Crucial for pipelined execution to break Write-After-Read
     * (WAR) dependencies, allowing Frame N writes to proceed concurrently with
     * Frame N-1 reads.
     */
    Previous = 1 << 5,

    /* --- Concurrency & Conflict Resolution --- */
    /* * Atomic: Declares the System will only mutate the chunk using hardware
     * atomic operations (e.g., std::atomic_ref or GPU atomics). Multiple
     * Systems can hold Write | Atomic concurrently. Highly relevant for
     * mutating the sparse matrix bitmap in the EntityDatabase.
     */
    Atomic = 1 << 6,

    /* * Accumulate: Data-parallel tasks write to thread/group-local temporary
     * memory chunks. The scheduler implicitly inserts a deterministic reduction
     * task to sum/merge the local chunks into the final component data array
     * before subsequent dependent tasks begin.
     */
    Accumulate = 1 << 7,

    /* --- Structural Mutation --- */
    /* * Deferred: Used for structural changes (e.g., adding/removing components
     * or entities). Grants a thread-local command buffer chunk handle. The
     * scheduler guarantees a cyclic synchronization point (e.g.,
     * `SystemMergeStructuralChanges`) to sequentially apply the mutations to
     * the EntityDatabase sparse matrix.
     */
    Deferred = 1 << 8,
};
} // namespace usagi::manifold

template <>
struct usagi::EnableBitMaskOperators<usagi::runtime::executive::DataAccessFlags>
    : std::true_type
{
};
