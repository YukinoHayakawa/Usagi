#pragma once

#include "StaticReflection.hpp"

namespace usagi::meta::reflection
{
// https://en.cppreference.com/w/cpp/language/lambda.html
// The lambda expression is a prvalue expression of unique
// unnamed non-union non-aggregate class type, known as closure type.
template <typename T>
concept Closure =
    !std::meta::is_union_type(^^T) && !std::meta::is_aggregate_type(^^T) &&
    std::meta::is_class_type(^^T) && !std::meta::has_identifier(^^T);

namespace static_tests
{
constexpr auto a_lambda = [](int) {
};

struct a_class
{
};

static_assert(Closure<decltype(a_lambda)>);
static_assert(!Closure<a_class>);
} // namespace static_tests
} // namespace usagi::meta::reflection
