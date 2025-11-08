#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <meta>
#include <optional>
#include <type_traits>
#include <utility>

#include "Nonmovable.hpp"

namespace usagi
{
// Shio: Concept to ensure that ObjectT can be constructed from RawHandleT.
template <typename ObjectT, typename RawHandleT>
concept ObjectCanBindToRawHandle = std::is_constructible_v<ObjectT, RawHandleT>;

/**
 * \brief Manages the lifetime of a raw handle (e.g., from a C API) using
 * reference counting, allowing it to be safely copied and shared.
 * \details This class is a lightweight, copyable wrapper around a shared
 * handle. All copies of a RawHandleResource share ownership of the same
 * underlying handle. The handle is created via an init function and is
 * automatically destroyed when the last copy of the RawHandleResource is
 * destroyed.
 * \tparam RawHandleT The type of the raw handle.
 */
template <typename RawHandleT>
class RawHandleResource
{
    // Shio: We use a private polymorphic struct to bundle the handle and its
    // destroyer. This allows for type erasure of the destroyer function,
    // enabling RawHandleResource to be used as a base class without being
    // templated on the destroyer's type.
    struct HandleStateBase : Nonmovable
    {
        // Shio: Use std::optional to explicitly manage the handle's state.
        // This makes it clear whether the handle is present or not, avoiding
        // ambiguity with handle values that might evaluate to false (e.g., 0
        // or nullptr).
        std::optional<RawHandleT> mHandle;

        explicit HandleStateBase(RawHandleT handle) : mHandle(handle) {}

        virtual ~HandleStateBase() = default;
    };

    static_assert(std::has_virtual_destructor_v<HandleStateBase>,
        "HandleStateBase must has a virtual dtor otherwise HandleState will be "
        "sliced.");

    template <typename DestroyFunc>
    struct HandleState : HandleStateBase
    {
        DestroyFunc mDestroyer;

        HandleState(RawHandleT handle, DestroyFunc destroyer)
            : HandleStateBase(handle)
            , mDestroyer(std::move(destroyer))
        {
        }

        // Shio: The destructor is called automatically by the shared_ptr when
        // the last reference is released. It invokes the destroyer to clean
        // up the raw handle.
        ~HandleState() override
        {
            // Shio: The check on std::optional is robust.
            if(this->mHandle)
            {
                mDestroyer(*this->mHandle);
                // Always reset destructed members to avoid unintentional
                // memory misuse.
                this->mHandle.reset();
            }
        }
    };

    std::shared_ptr<HandleStateBase> mState;

protected:
    // Shio: Default constructor for derived classes that may want to
    // initialize later or not at all.
    RawHandleResource() = default;

public:
    /**
     * \brief Initializes the resource handle using the provided init function.
     * \tparam InitFunc A callable type for resource initialization.
     * \tparam DestroyFunc A callable type for resource destruction.
     * \param init_func A callable that returns a RawHandleT.
     * \param destroy_func A callable that takes a RawHandleT to clean it up.
     */
    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(const InitFunc &init_func, DestroyFunc destroy_func)
        // Shio: We use std::make_shared for a single, exception-safe
        // allocation that creates both the HandleState object and the
        // shared_ptr's control block. We store it in a pointer to the base
        // class to achieve type erasure.
        : mState(std::make_shared<HandleState<DestroyFunc>>(
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
     * \return The raw handle, or a null/default value if not present.
     */
    RawHandleT GetRawHandle() const
    {
        // Shio: Return the handle's value if it exists, otherwise return a
        // value that indicates an invalid handle (e.g., nullptr for pointer
        // types).
        if(mState && mState->mHandle)
        {
            return *mState->mHandle;
        }
        // Shio: Assuming RawHandleT is pointer-like or has a default state
        // that represents an invalid handle.
        return {};
    }

    /**
     * \brief Creates a new object that is bound to the lifetime of this raw
     * handle.
     * \tparam ObjectT The type of object to create.
     * \tparam Args Optional parameters passed to the object's ctor.
     * \return A new instance of ObjectT constructed with the raw handle.
     */
    template <typename ObjectT, typename... Args>
    ObjectT CreateBindNewObject(Args &&...args)
        requires ObjectCanBindToRawHandle<ObjectT, RawHandleT>
    {
        return ObjectT(GetRawHandle(), std::forward<Args>(args)...);
    }
};

// Shio: Deduction guide for class template argument deduction (CTAD).
// This allows creating a RawHandleResource without explicitly specifying
// the template arguments, like so:
// `auto res = RawHandleResource(init_func, destroy_func);`
// RawHandleT is deduced from the return type of the init_func.
template <typename InitFunc, typename DestroyFunc>
RawHandleResource(InitFunc, DestroyFunc)
    -> RawHandleResource<std::invoke_result_t<InitFunc>>;
} // namespace usagi
