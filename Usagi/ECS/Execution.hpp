#pragma once

#include <algorithm>
#include <execution>

#include "EntityManager.hpp"

namespace usagi::ecs
{
class Execution
{
    EntityManager *mEntityManager = nullptr;

    template <typename... Components, typename ExecPolicy, typename Func>
    void executeTask(ExecPolicy policy, Func &&func)
    {
        auto streams = mEntityManager->componentStreams<Components...>();
        std::for_each(
            policy,
            mEntityManager->entityFilteredBegin<Components...>(),
            mEntityManager->entityFilteredEnd<Components...>(),
            [&](Entity &e) {
                std::apply(func, std::make_tuple(
                    std::ref(std::get<ComponentStream<Components>>(streams)
                        .seek(e.id))...
                ));
            }
        );
    }

public:
    template <typename... Components, typename Func>
    void parallel(Func &&func)
    {
        executeTask<Components...>(std::execution::par, func);
    }

    template <typename... Components, typename Func>
    void sequential(Func &&func)
    {
        executeTask<Components...>(std::execution::seq, func);
    }
};
}
