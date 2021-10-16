#pragma once

#include <memory>

namespace usagi
{
class Task;

class TaskExecutor
{
public:
    virtual ~TaskExecutor() = default;

    virtual void submit(std::unique_ptr<Task> task) = 0;
};
}
