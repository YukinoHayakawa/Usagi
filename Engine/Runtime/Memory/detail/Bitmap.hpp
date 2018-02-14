#pragma once

#include <vector>

namespace yuki::memory::detail
{
/**
 * \brief Tracks the availability of a fixed-size contiguous space by
 * uniformly dividing it into blocks. These blocks are minimum allocation
 * units, each represented by a bit. The exact size of the blocks are
 * ignored, and should be concerned about by the users of this class.
 */
class Bitmap
{
    /**
     * \brief Bytes is chosen instead of bits for its simplicity to handle.
     */
    enum class Block : unsigned char
    {
        FREE = 0,
        USED = 1,
        USED_BEGIN = 2,
    };

    std::vector<Block> mBitmap;

    std::vector<Block>::iterator determineScanningBegin(std::size_t start_block);

public:
    Bitmap() = default;
    explicit Bitmap(std::size_t num_blocks);

    void reset(std::size_t num_blocks);

    /**
     * \brief Allocate contiguous blocks. A starting position can be
     * specified to improve data locality. If the starting block is not
     * the beginning of a free block sequence, this requirement is ignored
     * and the allocation starts from the first block. If the allocation
     * request cannot be satisfied, a std::bad_alloc will be thrown.
     * \param num_blocks Must be positive, otherwise throws std::invalid_argument.
     * \param start_block 
     * \return The index of first allocated block.
     */
    std::size_t allocate(std::size_t num_blocks, std::size_t start_block = 0);

    /**
     * \brief Mark the block sequence started from the specified block as
     * free. If the specified block is not a beginning of such a sequence
     * or is already free, a std::invalid_argument is thrown.
     * \param start_block 
     */
    void deallocate(std::size_t start_block);

    std::size_t blockCount() const;
};
}
