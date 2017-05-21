#pragma once

namespace yuki
{
namespace reflection
{
namespace detail
{

template <typename>
struct extract_template;

template <template <typename...> typename Template, typename... TemplateArguments>
struct extract_template<Template<TemplateArguments...>>
{
    
};

template <typename T>
struct template_class_traits
{
    
};

}
}
}
