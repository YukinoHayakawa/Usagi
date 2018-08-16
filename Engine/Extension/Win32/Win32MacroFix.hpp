#pragma once

// Undefines problematic macros
// Include after including Windows headers

#ifdef DELETE
#   undef DELETE
#endif

#ifdef near
#   undef near
#endif

#ifdef far
#   undef far
#endif

#ifdef FAR
#   undef FAR
#endif

#ifdef NEAR
#   undef NEAR
#endif
