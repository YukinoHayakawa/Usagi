#pragma once

#include <memory>

namespace usagi
{
class Task;

class TaskExecutor
{
public:
    virtual ~TaskExecutor() = default;

    constexpr static std::uint64_t INVALID_TASK = -1;

    // A task id will be returned for tracking its status.
    virtual std::uint64_t submit(
        std::unique_ptr<Task> task,
        std::uint64_t wait_on = INVALID_TASK) = 0;
};
}
