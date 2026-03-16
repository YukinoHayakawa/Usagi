#include "EngineOrchestrator.hpp"

namespace usagi::runtime::orchestrator
{
void EngineOrchestrator::push_error_handler(
    errors::RuntimeErrorHandlerNode *node) noexcept
{
    if(!node) return;
    node->next_handler = mErrorHandlerHead;
    mErrorHandlerHead  = node;
}

void EngineOrchestrator::pop_error_handler(
    const errors::RuntimeErrorHandlerNode *node) noexcept
{
    if(!node || !mErrorHandlerHead) return;

    if(mErrorHandlerHead == node)
    {
        mErrorHandlerHead = node->next_handler;
        return;
    }

    errors::RuntimeErrorHandlerNode *current = mErrorHandlerHead;
    while(current->next_handler)
    {
        if(current->next_handler == node)
        {
            current->next_handler = node->next_handler;
            return;
        }
        current = current->next_handler;
    }
}

void EngineOrchestrator::dispatch_error(
    const errors::ErrorContext &context) noexcept
{
    errors::RuntimeErrorHandlerNode *current = mErrorHandlerHead;
    while(current)
    {
        if(current->handler)
        {
            errors::ErrorHandlerResult result = current->handler(context);
            if(result == errors::ErrorHandlerResult::Abort)
            {
                // The handler has signaled a clean abort for this Orchestrator
                // instance.
                return;
            }
            if(result == errors::ErrorHandlerResult::Swallow)
            {
                // The error was fully mitigated for this Orchestrator.
                return;
            }
        }
        current = current->next_handler;
    }
}
} // namespace usagi::runtime::orchestrator
