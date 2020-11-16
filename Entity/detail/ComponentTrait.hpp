#pragma once

#include <Usagi/Entity/Component.hpp>

namespace usagi
{
template <typename C>
concept TagComponent = Component<C> && requires
{
    typename C::TagComponent;
};
}

#define USAGI_DECL_TAG_COMPONENT(name) \
    struct name { using TagComponent = void; } \
/**/
