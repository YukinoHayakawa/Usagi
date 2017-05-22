#include <Usagi/Preprocessor/tuple.hpp>

static_assert(YUKI_TUPLE_IS_EMPTY(()) == 1, "");
static_assert(YUKI_TUPLE_IS_EMPTY((foo)) == 0, "");
static_assert(YUKI_TUPLE_IS_EMPTY((foo, bar)) == 0, "");
static_assert(YUKI_TUPLE_IS_EMPTY(((foo), )) == 0, "");
static_assert(YUKI_TUPLE_IS_EMPTY((, )) == 0, "");
