#pragma once

#include <memory>
#include <vector>
#include <optional>

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
class Task;

class TaskExecutor : Noncopyable
{
protected:
    static void run_task(Task &task);

public:
    virtual ~TaskExecutor() = default;

    constexpr static std::uint64_t INVALID_TASK = -1;

    // A task id will be returned for tracking its status.
    virtual std::uint64_t submit(
        std::unique_ptr<Task> task,
        std::optional<std::vector<std::uint64_t>> wait_on = { }) = 0;
};
}
