#pragma once

#include <Usagi/Core/Exception.hpp>

#include "ComponentStorage.hpp"

namespace usagi::ecs
{
template <typename Component>
class ComponentStream
{
    using StorageT = ComponentStorage<Component>;
    StorageT &mStorage;
    typename StorageT::ComponentIterator mCursor;

public:
    ComponentStream(ComponentStorage<Component> &storage)
        : mStorage(storage)
        , mCursor(mStorage.components.begin())
    {
    }

    Component & seek(EntityId id)
    {
        while(mCursor != mStorage.components.end())
        {
            if(mCursor->entity == id)
                return mCursor->component_data;
            if(mCursor->entity > id)
                break;
            ++mCursor;
        }
        USAGI_THROW(std::logic_error(
            "seeking component not belonging to the specified entity."));
    }
};
}
