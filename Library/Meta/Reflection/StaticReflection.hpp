#pragma once

// This header aims to improve readability for C++26 reflective source code when
// using Resharper C++ or Rider.
#ifdef __RESHARPER__
// Simply ignore the reflection operator which Resharper doesn't recognize.
// clang-format off
// Um. It seems `^^` are treated as two separate tokens so this doesn't really
// work.
// #define ^^
// clang-format on
// Gemini told me this trick, which works both before a function declaration
// **and** a `{}` block. Genuis one.
// https://gemini.google.com/app/a06090fa5d9a2faa
// Though I still cannot make `template for` look better.
#define consteval extern "C"
// Include all reflection features. This would actually cause a side effort
// of causing all `__has_feature(...)` to 1, but it doesn't matter since it
// won't leak to building process.
#define __has_feature(reflection) 1
// #define decltype(...) int
#endif
// Actually include the header.
#include <meta>
#ifdef __RESHARPER__
namespace std::__1::meta::reflection_v2
{
// This will make the symbol `std::meta::info` ambiguous, but it still looks
// better than full of red underlines.
struct info;
} // namespace std::__1::meta::reflection_v2

// A dummy definition of `__metafunction` for binary tests.
#define __metafunction(name, ...)                                             \
    [] -> bool {                                                              \
        using namespace std::__1::meta::reflection_v2::detail;                \
        if(name == __metafn_has_identifier) return std::declval<bool>();      \
        if(name >= __metafn_is_public && name <= __metafn_is_user_declared)   \
            return std::declval<bool>();                                      \
        if(name >= __metafn_has_ellipsis_parameter &&                         \
           name <= __metafn_is_function_parameter)                            \
            return std::declval<bool>();                                      \
        if(name == __metafn_is_annotation) return std::declval<bool>();       \
        if(name == __metafn_is_accessible) return std::declval<bool>();       \
        if(name == __metafn_is_access_specified) return std::declval<bool>(); \
        return false;                                                         \
    }()
namespace std::meta = std::__1::meta::reflection_v2;
// #undef decltype
#endif
