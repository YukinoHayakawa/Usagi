#pragma once

#include <array>
#include <cstddef>

#include "Alignment.hpp"
#include "Size.hpp"

namespace usagi
{
template <std::size_t MaxSize = 64>
class MemoryArena
{
	std::array<char, MaxSize> mMemory { };
	SmallestCapableType<MaxSize> mCursor { };

public:
	void * allocate(std::size_t size)
	{
	    if(mCursor + size > mMemory.size())
			return nullptr; // or throw bad_alloc?
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
			return nullptr; // or throw bad_alloc?
		const auto mem = static_cast<T *>(&mMemory[cursor_aligned]);
		std::construct_at(mem, std::forward<Args>(args)...);
		mCursor = cursor_next;
        return mem;
	}
};

using MemoryArenaDefaultSize = MemoryArena<>;
}
