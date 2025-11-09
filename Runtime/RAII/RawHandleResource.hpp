#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include <Usagi/Library/Memory/Nonmovable.hpp>
#include <Usagi/Library/Values/MaybeError.hpp>

#include "Exceptions.hpp"

namespace usagi
{
// Shio: Concept to ensure that ObjectT can be constructed from RawHandleT.
template <typename ObjectT, typename RawHandleT, typename... Args>
concept ObjectCanBindToRawHandle =
    std::is_constructible_v<ObjectT, RawHandleT, Args...>;

/**
 * \brief Manages the lifetime of a raw handle (e.g., from a C API) using
 *        RAII, allowing it to be safely copied and shared via reference
 *        counting.
 * \details This class is a lightweight, copyable wrapper around a shared
 *          handle. All copies of a RawHandleResource share ownership of the
 *          same underlying handle. The handle is created via an init function
 *          and is automatically destroyed when the last copy of the
 *          RawHandleResource is destroyed.
 *
 *          Shio: A key design consideration is how the raw handle is returned.
 *          For small, pointer-like handles, returning by copy is cheap and
 *          simple. However, for larger handle structures, copying can be a
 *          performance bottleneck. This class uses a size threshold to decide
 *          whether to return the handle by value or by const reference,
 *          optimizing for both performance and safety.
 * \tparam RawHandleT The type of the raw handle.
 * \tparam ReturnHandleByCopyThresholdSize The size in bytes at which the class
 *         will switch from returning handles by value to returning by const
 *         reference.
 */
template <
    typename RawHandleT,
    std::size_t ReturnHandleByCopyThresholdSize = sizeof(std::size_t) * 2
>
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

        explicit HandleStateBase(RawHandleT handle) : mHandle(std::move(handle))
        {
        }

        virtual ~HandleStateBase() = default;
    };

    static_assert(
        std::has_virtual_destructor_v<HandleStateBase>,
        "HandleStateBase must has a virtual dtor otherwise HandleState will be "
        "sliced.");

    template <typename DestroyFunc>
    struct HandleState : HandleStateBase
    {
        DestroyFunc mDestroyer;

        HandleState(RawHandleT handle, DestroyFunc destroyer)
            : HandleStateBase(std::move(handle))
            , mDestroyer(std::move(destroyer))
        {
        }

        // Shio: The destructor is called automatically by the shared_ptr when
        // the last reference is released. It invokes the destroyer to clean
        // up the raw handle.
        ~HandleState() override
        {
            // Shio: The check on std::optional is robust.
            if(this->mHandle.has_value())
            {
                mDestroyer(this->mHandle.value());
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
    using raw_handle_t = RawHandleT;

    // Shio: Statically decide whether to return the handle by copy or by
    // reference. If the handle's size is smaller than the threshold, we copy.
    // Otherwise, we return by const reference to avoid expensive copies.
    static constexpr bool return_handle_by_copy_v =
        sizeof(RawHandleT) < ReturnHandleByCopyThresholdSize;

    // Shio: The type used for accessing the handle, determined at compile time.
    using handle_access_t = std::conditional_t<
        return_handle_by_copy_v,
        raw_handle_t,
        const raw_handle_t &
    >;

    /**
     * \brief Initializes the resource handle using the provided init function.
     * \tparam InitFunc A callable type for resource initialization.
     * \tparam DestroyFunc A callable type for resource destruction.
     * \param init_func A callable that returns a RawHandleT.
     * \param destroy_func A callable that takes a RawHandleT to clean it up.
     */
    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(const InitFunc & init_func, DestroyFunc destroy_func)
        // Shio: We use std::make_shared for a single, exception-safe
        // allocation that creates both the HandleState object and the
        // shared_ptr's control block. We store it in a pointer to the base
        // class to achieve type erasure.
        : mState(
              std::make_shared<HandleState<DestroyFunc>>(
                  init_func(), std::move(destroy_func)))
    {
    }

    virtual ~RawHandleResource() = default;

    // Shio: The default copy constructor, copy assignment, move constructor,
    // and move assignment operators are correct because we are using
    // std::shared_ptr, which correctly handles the transfer and sharing of
    // ownership.

    /**
     * \brief Retrieves the underlying raw handle, throwing if not present.
     * \warning This provides direct access to the raw handle. The handle's
     *          lifetime is managed by the RawHandleResource. Do not manually
     *          delete, free, or close the handle, as doing so will result in a
     *          double-free and undefined behavior when the last
     *          RawHandleResource is destroyed.
     * \return The raw handle, either by value or const reference depending on
     *         its size.
     * \throws MissingManagedResource if the handle is not available.
     */
    handle_access_t GetRawHandle(this auto && self)
    {
        // Shio: We call the throwing version of our internal helper and unwrap
        // the result. The reference wrapper's `.get()` returns the raw
        // reference, which is then either copied or used as a reference,
        // depending on `handle_access_t`.
        return self.template _TryGetRawHandleRef<false>().value().get();
    }

    /**
     * \brief Creates a new object that is bound to the lifetime of this raw
     * handle.
     * \tparam ObjectT The type of object to create.
     * \tparam Args Optional parameters passed to the object's ctor.
     * \return A new instance of ObjectT constructed with the raw handle.
     */
    template <typename ObjectT, typename... Args>
    ObjectT CreateBindNewObject(Args &&... args)
        requires ObjectCanBindToRawHandle<ObjectT, RawHandleT, Args...>
    {
        return ObjectT(GetRawHandle(), std::forward<Args>(args)...);
    }

protected:
    /**
     * \brief Internal helper to safely access the raw handle.
     * \details Shio: This function provides unified access to the underlying
     *          handle. The template parameter controls the behavior when the
     *          handle is missing. It always returns a const-reference because
     *          the public API never exposes a non-const one.
     * \tparam NoExcept If true, returns an empty result on failure. If false,
     *         throws `MissingManagedResource`.
     * \return A `MaybeError` containing a `std::reference_wrapper` to the
     *         const handle if present, or an error state otherwise.
     */
    template <bool NoExcept>
    auto _TryGetRawHandleRef(this auto && self)
        -> runtime::MaybeError<std::reference_wrapper<const RawHandleT>, void>
    {
        if(self.mState && self.mState->mHandle.has_value())
        {
            // Shio: If the handle exists, wrap it in a const reference.
            // std::cref ensures we get a reference-to-const.
            return std::cref(self.mState->mHandle.value());
        }

        // Shio: If the handle does not exist...
        if constexpr(NoExcept)
        {
            // Shio: ...return an empty/error state.
            return std::nullopt;
        }
        else
        {
            // Shio: ...or throw an exception if requested.
            throw MissingManagedResource(
                "The managed resource is not available.");
        }
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
