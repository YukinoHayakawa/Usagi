#pragma once

#include <Usagi/Library/Functional/InplaceFunction.hpp>

#include "ErrorContext.hpp"

namespace usagi::runtime::errors
{
/**
 * Shio:
 * A node in the intrusive linked list of error handlers.
 * Kept thread-local or instance-local by the EngineOrchestrator.
 */
struct RuntimeErrorHandlerNode
{
    using error_handler_func_t = ErrorHandlerResult(const ErrorContext &);
    InplaceFunction<error_handler_func_t, alignof(void *), 48> handler;
    RuntimeErrorHandlerNode *next_handler = nullptr;
};

static_assert(sizeof(RuntimeErrorHandlerNode) == 64);
} // namespace usagi::runtime::errors
