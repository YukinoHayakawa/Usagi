#pragma once

#include <algorithm>
#include <ranges>

#include <Usagi/Library/Meta/Reflection/StaticReflection.hpp>
#include <Usagi/Library/Meta/Reflection/MakeStatics.hpp>

namespace usagi::meta::reflection
{
template <std::meta::info SortFunc, typename... Ts>
    requires (std::meta::is_function(SortFunc))
consteval auto sorted_types()
{
    std::array<std::meta::info, sizeof...(Ts)> sorted_types { ^^Ts... };
    std::ranges::sort(sorted_types, std::ranges::less { }, [](auto && refl) {
        return [:SortFunc:](refl);
    });
    return sorted_types;
}

template <typename... Ts>
consteval auto sorted_types_by_names()
{
    return sorted_types<^^std::meta::display_string_of, Ts...>();
}

template <typename... Ts>
consteval auto unique_type_list_sorted_by_names()
{
    // todo: bug - unique somehow returns an empty subrange, so we gotta do it
    //   ourselves.
    // const auto unique_range = std::ranges::unique(sorted_types);
    // return std::define_static_array(unique_range);

    constexpr auto sorted_types_ = sorted_types_by_names<Ts...>();
    std::array<std::meta::info, sizeof...(Ts)> unique_types { };
    std::size_t                                num_unique_types = 0;
    std::size_t                                i                = 0;
    template for(constexpr auto t : sorted_types_)
    {
        // push the first which is always unique.
        if(num_unique_types == 0)
        {
            unique_types[num_unique_types++] = sorted_types_[i];
        }
        else
        {
            if(unique_types[num_unique_types - 1] != sorted_types_[i + 1])
            {
                unique_types[num_unique_types++] = sorted_types_[++i];
            }
            else
            {
                ++i;
            }
        }
    }
    return make_static_subarray(unique_types, num_unique_types);
}

template <typename T, typename... Ts>
consteval bool is_type_in_list()
{
    constexpr std::array types { ^^Ts... };
    return std::ranges::find(types, ^^T) != types.end();
}

template <std::meta::info Refl>
concept TypeOrTypeAlias =
    std::meta::is_type_alias(Refl) || std::meta::is_type(Refl);

template <std::meta::info Refl>
concept TypeOrTemplate = TypeOrTypeAlias<Refl> || std::meta::is_template(Refl);

template <std::meta::info Refl>
concept TemplateInstantiation = std::meta::has_template_arguments(Refl);

// this doesn't compile
// requires { { std::meta::template_of(Refl) }; };

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
}
} // namespace static_tests
} // namespace usagi::meta::reflection
