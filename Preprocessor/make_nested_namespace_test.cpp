#include <Preprocessor/make_nested_namespace.hpp>

YUKI_MAKE_NESTED_NAMESPACE((foo, bar, baz), (constexpr int hello = 5;))

static_assert(foo::bar::baz::hello == 5, "make_nested_ns");

YUKI_MAKE_NESTED_NAMESPACE((), (constexpr int world = 10;))

static_assert(world == 10, "make_nested_ns in root ns");
