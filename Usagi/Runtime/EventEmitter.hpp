#pragma once

#include <vector>
#include <algorithm>

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
template <typename ListenerT>
class EventListener;

template <typename ListenerT>
class EventEmitter : Noncopyable
{
    friend class EventListener<ListenerT>;

    void eraseEventListener(ListenerT *listener)
    {
        mEventListeners.erase(
            std::remove(
                mEventListeners.begin(), mEventListeners.end(), listener
            ), mEventListeners.end()
        );
    }

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
    virtual ~EventEmitter()
    {
        // emitter-listener relationships are automatically cleaned up
        // when either is destroyed.
        for(auto && listener : mEventListeners)
        {
            listener->removeRegisteredEmitter(this);
        }
    }

    void addEventListener(ListenerT *listener)
    {
        listener->addRegisteredEmitter(this);
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
        listener->removeRegisteredEmitter(this);
        eraseEventListener(listener);
    }
};
}
