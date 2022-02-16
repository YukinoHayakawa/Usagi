#pragma once

#include <array>
#include <cstddef>

#include "Alignment.hpp"

namespace usagi
{
template <std::size_t MaxSize = 128>
class MemoryArenaBase
{
	std::array<char, MaxSize> mMemory { };
    std::size_t mCursor { };

public:
	void * allocate(std::size_t size)
	{
	    if(mCursor + size > mMemory.size())
			throw std::bad_alloc();
		const auto mem = &mMemory[mCursor];
		mCursor += size;
        return mem;
	}

	template <typename T, typename... Args>
	T * create(Args &&... args)
	    // requires std::is_trivially_destructible_v<T>
	{
	    const auto alignment = std::alignment_of_v<T>;
        const auto cursor_aligned = align_up(mCursor, alignment);
        const auto cursor_next = cursor_aligned + sizeof(T);
		if(cursor_next > mMemory.size())
			throw std::bad_alloc();
		const auto mem = reinterpret_cast<T *>(&mMemory[cursor_aligned]);
		std::construct_at(mem, std::forward<Args>(args)...);
		mCursor = cursor_next;
        return mem;
	}
};

// todo: introduce a base to allow other sizes
using MemoryArena = MemoryArenaBase<>;
}
