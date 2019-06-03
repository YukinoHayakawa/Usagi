#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace usagi
{
inline void debugBreak()
{
#ifdef _WIN32
	if(IsDebuggerPresent())
	{
		__debugbreak();
	}
#else
	__builtin_trap();
#endif
}
}
