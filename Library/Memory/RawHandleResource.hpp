#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

#include "Nonmovable.hpp"

namespace usagi
{
// Shio: Concept to ensure that ObjectT can be constructed from RawHandleT.
template <typename ObjectT, typename RawHandleT>
concept CanBindToRawHandle = std::is_constructible_v<ObjectT, RawHandleT>;

/**
 * \brief Manages the lifetime of a raw handle (e.g., from a C API) using
 * reference counting, allowing it to be safely copied and shared.
 * \details This class is a lightweight, copyable wrapper around a shared
 * handle. All copies of a RawHandleResource share ownership of the same
 * underlying handle. The handle is created via an init function and is
 * automatically destroyed when the last copy of the RawHandleResource is
 * destroyed.
 * \tparam RawHandleT The type of the raw handle.
 * \tparam InitFunc A callable type for resource initialization.
 * \tparam DestroyFunc A callable type for resource destruction.
 */
template <typename RawHandleT, typename InitFunc, typename DestroyFunc>
class RawHandleResource
{
    // Shio: We use a private struct to bundle the handle and its destroyer.
    // A shared_ptr to this struct provides non-intrusive reference counting.
    // This is the standard C++ approach for managing shared ownership of a
    // resource without modifying the resource's class itself. An alternative
    // like boost::intrusive_ptr would require the managed object to contain
    // the reference counter, which is not possible for raw handles from C APIs.
    // Shio: This struct should not be copyable or movable, as its
    // lifetime is exclusively managed by the shared_ptr.
    struct HandleState : Nonmovable
    {
        RawHandleT  mHandle;
        DestroyFunc mDestroyer;

        // Shio: The constructor takes ownership of the handle and the
        // destroyer.
        HandleState(RawHandleT handle, DestroyFunc destroyer)
            : mHandle(handle)
            , mDestroyer(std::move(destroyer))
        {
        }

        // Shio: The destructor is called automatically by the shared_ptr when
        // the last reference is released. It invokes the destroyer to clean
        // up the raw handle.
        ~HandleState()
        {
            mDestroyer(mHandle);
        }
    };

    std::shared_ptr<HandleState> mState;

public:
    /**
     * \brief Initializes the resource handle using the provided init function.
     * \param init_func A callable that returns a RawHandleT.
     * \param destroy_func A callable that takes a RawHandleT to clean it up.
     */
    explicit RawHandleResource(InitFunc &&init_func, DestroyFunc destroy_func)
        // Shio: We use std::make_shared for a single, exception-safe
        // allocation that creates both the HandleState object and the
        // shared_ptr's control block.
        : mState(std::make_shared<HandleState>(
              init_func(), std::move(destroy_func)))
    {
    }

    // Shio: The default copy constructor, copy assignment, move constructor,
    // and move assignment operators are correct because we are using
    // std::shared_ptr, which correctly handles the transfer and sharing of
    // ownership.

    /**
     * \brief Retrieves the underlying raw handle.
     * \warning This provides direct access to the raw handle. The handle's
     * lifetime is managed by the RawHandleResource. Do not manually delete,
     * free, or close the handle, as doing so will result in a double-free
     * and undefined behavior when the last RawHandleResource is destroyed.
     * \return The raw handle.
     */
    RawHandleT GetRawHandle() const
    {
        return mState->mHandle;
    }

    /**
     * \brief Creates a new object that is bound to the lifetime of this raw
     * handle.
     * \tparam ObjectT The type of object to create.
     * \return A new instance of ObjectT constructed with the raw handle.
     */
    template <typename ObjectT>
    ObjectT CreateBindNewObject()
        requires CanBindToRawHandle<ObjectT, RawHandleT>
    {
        return ObjectT(GetRawHandle());
    }
};

// Shio: Deduction guide for class template argument deduction (CTAD).
// This allows creating a RawHandleResource without explicitly specifying
// the template arguments, like so:
// `auto res = RawHandleResource(init_func, destroy_func);`
// RawHandleT is deduced from the return type of the init_func.
template <typename InitFunc, typename DestroyFunc>
RawHandleResource(InitFunc, DestroyFunc)
    -> RawHandleResource<std::invoke_result_t<InitFunc>, InitFunc, DestroyFunc>;
} // namespace usagi
