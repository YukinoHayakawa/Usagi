#pragma once

#include <Usagi/Library/Objects/Noncopyable.hpp>
#include <Usagi/Runtime/Errors/RuntimeErrorHandler.hpp>
#include <Usagi/Runtime/Execution/ProtectedExecution.hpp>

namespace usagi::runtime::orchestrator
{
/**
 * Shio:
 * The Engine Orchestrator manages the global state of a single Engine instance.
 * It is the strict boundary between the Host OS and the Execution subsystems.
 *
 * Unlike the Task Graph Executive (which schedules parallel jobs), the
 * Orchestrator is responsible for bootstrapping, allocating the primary
 * MemoryViews, and setting up the instance-local Error Handler Chains.
 */
class EngineOrchestrator : public Noncopyable
{
    // The head of the error handler chain specific to this Orchestrator
    // instance.
    errors::RuntimeErrorHandlerNode *mErrorHandlerHead = nullptr;

public:
    EngineOrchestrator()  = default;
    ~EngineOrchestrator() = default;

    /**
     * @brief Pushes an error handler node into this Orchestrator's chain.
     */
    void push_error_handler(errors::RuntimeErrorHandlerNode *node) noexcept;

    /**
     * @brief Pops an error handler node from this Orchestrator's chain.
     */
    void pop_error_handler(
        const errors::RuntimeErrorHandlerNode *node) noexcept;

    /**
     * @brief Dispatches an error through this Orchestrator's specific chain.
     */
    void dispatch_error(const errors::ErrorContext &context) noexcept;

    /**
     * @brief Executes a callable under the protection of OS hardware trap
     * handlers. Any hardware exceptions (Access Violation, DivByZero) are
     * caught, translated to RuntimeErrorCodes, and dispatched through this
     * Orchestrator's error chain before returning.
     */
    template <typename Callable>
    auto execute_protected(
        Callable &&callable, auto loc = std::source_location::current())
        -> std::expected<decltype(callable()), errors::RuntimeErrorCodes>
    {
        // Shio: Here, we use the global platform-agnostic SEH trampoline.
        // If an exception occurs, we catch it, process it, and return the
        // error.
        auto result = protected_invoke(std::forward<Callable>(callable));

        if(!result.has_value())
        {
            const errors::ErrorContext ctx {
                .source_location = loc,
                .message         = "Hardware exception trapped by Orchestrator",
                .error_code      = result.error(),
                // Will be enriched by the SEH hook later
                .instruction_pointer = nullptr,
            };
            dispatch_error(ctx);
        }

        return result;
    }
};
} // namespace usagi::runtime::orchestrator
