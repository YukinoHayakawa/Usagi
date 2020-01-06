#pragma once

#include "ComponentFilter.hpp"

namespace usagi::ecs
{
template <
    typename Database,
    typename PermissionChecker,
    typename IncludeFilter,
    typename ExcludeFilter = ComponentFilter<>
>
class EntityDatabaseView
{
public:
    using DatabaseT = Database;
    using PermissionCheckerT = PermissionChecker;

private:
    DatabaseT &mBaseview;

public:



    auto begin()
    {

    }

    auto end()
    {

    }
};
}
