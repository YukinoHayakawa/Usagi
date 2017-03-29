#include "../string_literal.hpp"

using namespace yuki;

static_assert(make_string_literal("hello") == "hello", "==");
static_assert(make_string_literal("hello") == "hello\0hello", "==_1");

static_assert(make_string_literal("hello") != "hell", "!=");
static_assert(make_string_literal("hello") != "helloo", "!=_1");
static_assert(make_string_literal("hello") != "", "!=_2");
