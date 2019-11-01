#pragma once

#include "Component.hpp"

namespace usagi::ecs
{
template <Component... EnabledComponents>
class Entity
{
    using DatabaseT = EntityDatabase<EnabledComponents...>;

    DatabaseT &mDatabase;
    EntityId mId;
    DatabaseT::EntityPageT &mPage = mDatabase.entityPage(mId);

    template <Component T>
    std::size_t indexInPage() const
    {
        mPage.componentPageIndex()<T>
    }

public:
    template <Component T>
    T & operator()()
    {
        const auto cpi = mPage->componentPageIndex<T>();
        auto &cs = mDatabase->componentStorage<T>();
        return cs->page(cpi)[indexInPage()];
    }
};
}
