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
        // todo: fix data race of streams in all execution threads
        auto streams = mEntityManager->componentStreams<Components...>();
        std::for_each(
            policy,
            mEntityManager->entityFilteredBegin<Components...>(),
            mEntityManager->entityFilteredEnd<Components...>(),
            [&, streams](Entity &e) mutable {
                auto comp = std::make_tuple(
                    std::ref(std::get<ComponentStream<Components>>(streams)
                        .seek(e.id))...
                );
                std::apply(func, comp);
            }
        );
    }

public:
    Execution(EntityManager *entity_manager)
        : mEntityManager(entity_manager)
    {
    }

    template <typename... Components, typename Func>
    void parallel(Func &&func)
    {
        // todo impl parallel execution
        // executeTask<Components...>(std::execution::par, func);
        executeTask<Components...>(std::execution::seq, func);
    }

    template <typename... Components, typename Func>
    void sequential(Func &&func)
    {
        executeTask<Components...>(std::execution::seq, func);
    }
};
}
