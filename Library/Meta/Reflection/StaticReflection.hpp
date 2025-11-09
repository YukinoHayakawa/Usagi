#pragma once

// This header is a hack for IDE readability
#ifdef __RESHARPER__
// Include all reflection features. This would actually cause a side effort
// of causing all `__has_feature(...)` to 1, but it doesn't matter since it
// won't leak to building process.
#define __has_feature(reflection) 1
// Make the lines in `<meta>` look better.
#define __metafunction(name, ...) std::declval<decltype(name)>()
#endif
#include <meta>
#ifdef __RESHARPER__
// Don't know why but this ill-formed alias makes metafunctions locatable by
// the IDE.
using namespace std::meta = std::__1::meta::reflection_v2;
#endif
