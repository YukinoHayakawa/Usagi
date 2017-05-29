#pragma once

#ifdef _MSC_VER
#   define YUKI_EXPORT_DECL __declspec(dllexport)
#else
#   define YUKI_EXPORT_DECL
#endif
