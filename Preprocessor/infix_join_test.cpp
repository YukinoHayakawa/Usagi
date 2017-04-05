#include <Preprocessor/make_nested_namespace.hpp>
#include <Preprocessor/infix_join.hpp>

YUKI_MAKE_NESTED_NAMESPACE((foo, bar, baz), (constexpr int hello = 5;))

static_assert(YUKI_INFIX_JOIN(::, foo, bar, baz, hello) == 5, "infix_join");
