#pragma once

#include "EntityDatabaseInterface.hpp"

namespace usagi::ecs
{
template <
    EntityDatabaseInterface Database,
    PermissionChecker PermCheck,
    EntityFilter Filter
>
class EntityDatabaseView
{
public:
    using DatabaseT = Database;

private:
    Database &mBaseview;

public:

};
}
