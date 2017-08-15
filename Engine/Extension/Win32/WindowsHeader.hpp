#pragma once

/*
 * This file includes the Windows header and undef any macros conflicting with 
 * our codes.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DELETE
#undef near
#undef far
#undef FAR
#undef NEAR
