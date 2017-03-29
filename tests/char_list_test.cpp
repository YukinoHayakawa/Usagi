#include <type_traits>

#include "../char_list.hpp"

using namespace yuki;

typedef char_list<'h', 'e', 'l', 'l', 'o' > ref;
typedef char_list<'h', 'e', 'l', 'l', 'o', 0, 0 > ref2;
typedef YUKI_MAKE_CHAR_LIST("hello") from_string;
typedef YUKI_MAKE_CHAR_LIST_STRINGIFY(hello) stringified;

static_assert(std::is_same<from_string, ref>::value, "YUKI_MAKE_CHAR_LIST");
static_assert(std::is_same<stringified, ref>::value, "YUKI_MAKE_CHAR_LIST_STRINGIFY");
static_assert(std::is_same<ref, ref2::c_string_t>::value, "c_string_t");

static_assert(ref::length == 5, "length");
static_assert(ref::c_string_length == 5, "c_string_length");

static_assert(ref2::length == 7, "length_2");
static_assert(ref2::c_string_length == 5, "c_string_length_2");

static_assert(ref::to_string_literal() == "hello", "to_string_literal");
static_assert(ref2::to_string_literal() == "hello", "to_string_literal_2");
