#pragma once

#include <vector>
#include <algorithm>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
template <typename ListenerT>
class EventEmitter : Noncopyable
{
protected:
    std::vector<ListenerT*> mEventListeners;

    // todo: concept TS: use auto for param type.
    template <typename Handler>
    void forEachListener(Handler handler)
    {
        for(auto &&l : mEventListeners)
        {
            handler(l);
        }
    }

public:
    virtual ~EventEmitter() = default;

    void addEventListener(ListenerT *listener)
    {
        mEventListeners.push_back(listener);
    }

    bool hasEventListener(ListenerT *listener) const
    {
        return std::find(
            mEventListeners.begin(), mEventListeners.end(), listener
        ) != mEventListeners.end();
    }

    void removeEventListener(ListenerT *listener)
    {
        mEventListeners.erase(
            std::remove(
                mEventListeners.begin(), mEventListeners.end(), listener
            ), mEventListeners.end()
        );
    }
};
}
