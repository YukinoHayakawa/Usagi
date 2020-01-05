#pragma once

#include <utility>

namespace usagi
{
class RuntimeServiceProvider;
}

namespace usagi::ecs
{
template <typename T, typename EntityDb>
concept System = requires (T t, EntityDb db) {
    t.update(
        std::declval<RuntimeServiceProvider>(),
        std::declval<EntityDatabaseAccess<EntityDb, T>>()
    );
};
}
