#pragma once

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
class Element;

/**
 * \brief Note on event system:
 * Event system is purposed for decoupled communication among entities.
 * An entity is both an event sender and receiver. When an event is sent,
 * the event is first processed at the sender entity, then bubbled through
 * each ancestor till the root.
 * TODO: event capturing?
 * Use cases:
 * - Component registration
 *   When a component is added to an entity, an AddComponent event is sent.
 *   An event handler at the root receive this event and let each subsystem
 *   to inspect the entity and record it if it is of interest.
 * - Delayed event
 *   A handler can hold a timed event and resend it when appropriate.
 */
class Event : Noncopyable
{
    Element *mSource = nullptr;
    // todo: c++20, bool mBubbling : 1 = true;
    bool mBubbling : 1;
    bool mCanceled : 1;

    friend class Element;

    void setSource(Element *source) { mSource = source; }

public:
    Event();
    virtual ~Event() = default;

    Element * source() const { return mSource; }
    bool bubbling() const { return mBubbling; }
    bool canceled() const { return mCanceled; }

    /**
     * \brief Stop calling next event handler immediately after finishing
     * the current one.
     * Like stopImmediatePropagation() in JavaScript.
     */
    void cancel() { mCanceled = true; }

    /**
     * \brief After invoking all the event handlers at current level,
     * prevent the event from reaching the upper level.
     * Like stopPropagation() in JavaScript.
     */
    void stopBubbling() { mBubbling = false; }
};
}
