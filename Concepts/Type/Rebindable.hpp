#pragma once

namespace usagi
{
// This concept checks that an instantiated template class can return a
// type that instantiate the template for another type.
template <typename Instantiated, typename AnotherT>
concept Rebindable = requires {
    typename Instantiated::template rebind<AnotherT>;
};
}
