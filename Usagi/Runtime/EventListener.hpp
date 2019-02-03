#pragma once

#include <set>

#include <Usagi/Utility/Noncopyable.hpp>

#include "EventEmitter.hpp"

namespace usagi
{
template <typename ListenerT>
class EventListener : Noncopyable
{
    using Emitter = EventEmitter<ListenerT>;
    friend class Emitter;
    std::set<Emitter*> mRegistered;

    void addRegisteredEmitter(Emitter *emitter)
    {
        mRegistered.insert(emitter);
    }

    void removeRegisteredEmitter(Emitter *emitter)
    {
        mRegistered.erase(emitter);
    }

public:
    virtual ~EventListener()
    {
        // automatically unregister from emitters
        for(auto && emitter : mRegistered)
        {
            // ListenerT must inherit from this class
            emitter->eraseEventListener(static_cast<ListenerT*>(this));
        }
    }
};
}
