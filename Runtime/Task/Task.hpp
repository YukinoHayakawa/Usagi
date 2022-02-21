#pragma once

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
class Task : Noncopyable
{
public:
    virtual ~Task() = default;

    // Validate precondition of executing the task. If this method returns
    // false, the task will not be run and an exception will be thrown.
    virtual bool precondition() { return true; }

    virtual void on_started() { }
    virtual void run() = 0;
    virtual void on_finished() { }

    // Validate postcondition of executing the task. If this method returns
    // false, an exception will be thrown.
    virtual bool postcondition() { return true; }

    enum FinishedAction
    {
        FINISHED,
        RESCHEDULE,
    };
};
}
