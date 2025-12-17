#pragma once

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>

namespace usagi::meta::reflection
{
template <std::meta::info Refl>
concept TypeOrTypeAlias =
    std::meta::is_type_alias(Refl) || std::meta::is_type(Refl);

template <std::meta::info Refl>
concept TypeOrTemplate = TypeOrTypeAlias<Refl> || std::meta::is_template(Refl);

template <std::meta::info Refl>
concept TemplateInstantiation = std::meta::has_template_arguments(Refl);

// this doesn't compile
// requires { { std::meta::template_of(Refl) }; };

template <typename T>
consteval auto make_explicit_dealias()
{
    return std::meta::dealias(^^T);
}

template <std::meta::info Refl>
    requires TypeOrTemplate<Refl>
consteval auto get_dealiased_type_or_template()
{
    if constexpr(std::meta::is_type_alias(Refl))
    {
        return std::meta::dealias(Refl);
    }
    return Refl;
}

// Refl has to be something you want to confirm about.
// CmpRefl has to be a reflection of a type/type alias/template.
template <std::meta::info Refl, std::meta::info CmpRefl>
    requires (TypeOrTemplate<CmpRefl> || TemplateInstantiation<CmpRefl>)
consteval bool is_type_or_template_instantiation_of()
{
    if constexpr(TypeOrTypeAlias<Refl> && !TemplateInstantiation<Refl>)
    {
        return get_dealiased_type_or_template<Refl>() == CmpRefl;
    }
    // a template itself cannot be an instantiation
    else if constexpr(std::meta::is_template(Refl))
    {
        return false;
    }
    // Then it's a template instantiation
    else if constexpr(
        constexpr auto i = get_dealiased_type_or_template<Refl>();
        TemplateInstantiation<i>)
    {
        return std::meta::template_of(i) == CmpRefl;
    }
    return false;
}

namespace static_tests
{
template <typename>
struct _meta_type_x
{
};

consteval
{
    struct y
    {
    };

    struct w
    {
    };

    using z = y;

    static_assert(get_dealiased_type_or_template<^^z>() == ^^y);
    static_assert(get_dealiased_type_or_template<^^y>() == ^^y);
    static_assert(
        get_dealiased_type_or_template<^^_meta_type_x>() == ^^_meta_type_x
    );
    static_assert(is_type_or_template_instantiation_of<^^y, ^^y>());
    static_assert(!is_type_or_template_instantiation_of<^^y, ^^w>());
    static_assert(
        !is_type_or_template_instantiation_of<^^_meta_type_x, ^^_meta_type_x>()
    );
    static_assert(is_type_or_template_instantiation_of<
        ^^_meta_type_x<int>,
        ^^_meta_type_x
    >());

    static_assert(
        make_explicit_dealias<std::int32_t>() == std::meta::dealias(^^::int32_t)
    );
}
} // namespace static_tests
} // namespace usagi::meta::reflection
