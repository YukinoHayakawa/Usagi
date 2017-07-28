#include <Usagi/Preprocessor/make_nested_namespace.hpp>

namespace
{

YUKI_MAKE_NESTED_NAMESPACE((foo, bar, baz), (constexpr int hello = 5;))
YUKI_MAKE_NESTED_NAMESPACE((), (constexpr int world = 10;))
YUKI_MAKE_NESTED_NAMESPACE((curious), (constexpr int magical_world = 1000;))

}

static_assert(foo::bar::baz::hello == 5, "make_nested_ns");
static_assert(world == 10, "make_nested_ns in root ns");
static_assert(curious::magical_world == 1000, "() and (curious) have different sizes");
