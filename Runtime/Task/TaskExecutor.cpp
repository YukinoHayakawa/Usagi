#include "TaskExecutor.hpp"

#include <stdexcept>

#include "Task.hpp"

namespace usagi
{
void TaskExecutor::run_task(Task &task)
{
    if(!task.precondition())
        throw std::runtime_error("");
    task.on_started();
    task.run();
    task.on_finished();
    if(!task.postcondition())
        throw std::runtime_error("");
}
}
