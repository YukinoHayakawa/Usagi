#pragma once

#ifdef _MSC_VER
#   define USAGI_EXPORT_DECL __declspec(dllexport)
#else
#   define USAGI_EXPORT_DECL
#endif
