#pragma once

#include <Usagi/Library/Meta/Reflection.hpp>

namespace usagi::meta
{
/**
 * Shio:
 * Evaluates whether a given type reflection `type_r` is an instantiation of the
 * template `template_r`. Example:
 * `is_template_instantiation_of(^^std::vector<int>, ^^std::vector)` returns
 * true.
 */
template <std::meta::info TypeRefl, std::meta::info TemplateRefl>
consteval bool is_class_template_instantiation_of()
{
    if constexpr(!TypeOrTypeAlias<TypeRefl>)
    {
        return false;
    }
    else if constexpr(!std::meta::is_class_template(TemplateRefl))
    {
        return false;
    }
    else if constexpr(TemplateSpecialization<to_underlying<TypeRefl>()>)
    {
        return std::meta::template_of(to_underlying<TypeRefl>()) ==
            TemplateRefl;
    }
    else
    {
        return false;
    }
}

namespace details::static_tests
{
template <typename>
struct DummyTemplate
{
};

struct DummyStruct
{
};

static_assert(is_class_template_instantiation_of<
    ^^DummyTemplate<int>, ^^DummyTemplate
>());
static_assert(
    !is_class_template_instantiation_of<^^DummyStruct, ^^DummyTemplate>());
} // namespace details::static_tests
} // namespace usagi::meta
