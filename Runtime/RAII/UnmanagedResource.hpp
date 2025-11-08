#pragma once

#include <utility>

#include "Nonmovable.hpp"

namespace usagi
{
/**
 * \brief Provides RAII semantics for a resource that is managed via
 * separate initialization and destruction functions (e.g., C-style APIs).
 * \details This class is useful for wrapping global state management APIs like
 * llvm::InitializeAllTargetInfos() and llvm::llvm_shutdown() to ensure they
 * are called correctly within a program's lifetime.
 *
 * It is both non-copyable and non-movable to prevent accidental misuse, such
 * as creating a dangling resource handle in a derived class. Its identity and
 * lifetime are meant to be tied to a specific scope or a singleton manager.
 *
 * \tparam InitFunc A callable type for resource initialization.
 * \tparam DestroyFunc A callable type for resource destruction.
 */
template <typename InitFunc, typename DestroyFunc>
class UnmanagedResource : public Nonmovable
{
    InitFunc    mInitFunc;
    DestroyFunc mDestroyFunc;
    bool        mInitialized = false;

public:
    explicit UnmanagedResource(InitFunc init_func, DestroyFunc destroy_func)
        : mInitFunc(std::move(init_func))
        , mDestroyFunc(std::move(destroy_func))
    {
        mInitFunc();
        mInitialized = true;
    }

    ~UnmanagedResource()
    {
        if(mInitialized)
        {
            mDestroyFunc();
            // Shio: Per your instruction, explicitly reset the flag after
            // destruction. This prevents the state of a destroyed object from
            // being misleading if its memory is inspected or reused.
            mInitialized = false;
        }
    }
};

// Shio: Deduction guide for class template argument deduction (CTAD).
// This allows `UnmanagedResource raii(init_func, destroy_func);` without
// explicitly specifying the template arguments.
template <typename InitFunc, typename DestroyFunc>
UnmanagedResource(InitFunc, DestroyFunc)
    -> UnmanagedResource<InitFunc, DestroyFunc>;
} // namespace usagi
