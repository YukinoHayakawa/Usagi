#pragma once

#include <exception>
#include <expected>
#include <functional>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include <Usagi/Library/Memory/Construction.hpp>
#include <Usagi/Library/Memory/Nonmovable.hpp>
#include <Usagi/Library/Meta/Reflection/Members.hpp>
#include <Usagi/Library/Meta/Types/Traits.hpp>

#include "Exceptions.hpp"
#include "ResourceErrorCodes.hpp"

namespace usagi::runtime
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
 * \tparam AllowMutableHandleAccess If true, exposes a function to get a
 *         mutable reference to the handle. This is dangerous and should be
 *         used with extreme caution.
 * \tparam PerformCleanUpOnInitFuncThrows If true, the destroyer will be
 *         invoked on a partially constructed handle if the init function
 *         throws. This is dangerous and assumes the destroyer can handle this.
 * \tparam ReturnHandleByCopyThresholdSize The size in bytes at which the class
 *         will switch from returning handles by value to returning by const
 *         reference.
 */
template <
    /*
     * The nominal type of raw resource to be managed. It may be as small as a
     * pointer, or might be something large and have to be initialized by some
     * sophisticated process. It must not be a reference.
     */
    type_traits::NonReference RawHandleT,
    /*
     * Sometimes you'd like to have mutable access to the managed handle. But
     * this shouldn't be the norm.
     */
    bool                      AllowMutableHandleAccess = false,
    /*
     * If this is true, even `aligned_storage` is used, it will be first
     * initialized with placement new using default constructor of `RawHandleT`.
     * This is mainly for preventing having garbage values in case of failures
     * of internal states construction left on the heap. But if `RawHandleT`
     * doesn't have a default constructor, set this to false.
     */
    bool                      PerformDefaultConstructionOnInternalStorage =
        std::is_default_constructible_v<RawHandleT>,
    /*
     * `InitFunc()` is allowed to throw upon failing to construct the raw
     * resource. According to the principles of RAII, we should unwind and
     * perform clean up. But this is up to the nature of the raw handle being
     * managed. If there won't be anything to clean up, you can set this to
     * false.
     */
    bool        PerformCleanUpOnInitFuncThrows  = true,
    /*
     * A naive threshold for deciding when to return a copy of the handle
     * preventing too much copying overhead when `GetRawHandle()` is called.
     */
    std::size_t ReturnHandleByCopyThresholdSize = sizeof(std::size_t) * 2
>
// todo: if we are using aligned storage, it might be hard to get
//   DestroyFunc right. So currently we enforce this.
    requires std::is_default_constructible_v<RawHandleT>
class RawHandleResource
{
public:
    using raw_handle_t = RawHandleT;

    // ********************************************************************* //
    //                          Return Type Handling                         //
    // ********************************************************************* //

    // Shio: Statically decide whether to return the handle by copy or by
    // reference. If the handle's size is smaller than the threshold, we copy.
    // Otherwise, we return by const reference to avoid expensive copies.
    static constexpr bool return_handle_by_copy_v =
        sizeof(raw_handle_t) < ReturnHandleByCopyThresholdSize;

    // Shio: The type used for accessing the handle, determined at compile time.
    using handle_access_t = std::conditional_t<
        return_handle_by_copy_v,
        raw_handle_t,
        const raw_handle_t &
    >;

    // If `return_handle_by_copy_v == true`, it's impossible to get a mutable
    // reference.
    using handle_mutable_access_t =
        std::conditional_t<return_handle_by_copy_v, void, raw_handle_t &>;

    static constexpr bool mutable_handle_reference_exposed_v =
        !return_handle_by_copy_v && AllowMutableHandleAccess;

    // ********************************************************************* //
    //                          Storage Type Handling                        //
    // ********************************************************************* //

    /*
     * There are two ways for providing a storage for `raw_handle_t`. One way
     * is to simply put it in `HandleStateBase`. This somehow requires it to
     * have a default constructor, which might be, well, overly strict, because
     * we are using `InitFunc` as an external constructor anyway. So the
     * alternative is to use `std::aligned_storage` to provide a raw byte block
     * to it, and let `InitFunc` fill the bytes. In this case, there must be a
     * corresponding **destructor** for incompletely constructed raw resource.
     * So I assume that if `PerformCleanUpOnInitFuncThrows == true`, the user
     * of this class must be prepared for getting every byte in the storage
     * correct.
     */
    static constexpr bool is_using_raw_storage_block_v =
        PerformCleanUpOnInitFuncThrows;

    // Only when we are using `make_shared_for_overwrite` we want raw
    // `aligned_storage_t`. Otherwise, a plain `raw_handle_t` is enough.
    /*
     * Note that if `aligned_storage` is used, you are fully responsible for
     * getting every byte in it correct. ... Also destroying them.
     */
    using raw_storage_t = std::conditional_t<
        is_using_raw_storage_block_v,
        // Usually, this will simply a block of `char[]` prepended with
        // something forcing the alignment.
        // todo: do we really have to have that dummy alignment value?
        //   it kinda wastes memory.
        std::aligned_storage_t<sizeof(raw_handle_t)>,
        raw_handle_t
    >;

private:
    // Shio: We use a private polymorphic struct to bundle the handle and its
    // destroyer. This allows for type erasure of the destroyer function,
    // enabling RawHandleResource to be used as a base class without being
    // templated on the destroyer's type. The shared_ptr manages the lifetime
    // of this state, enabling shared ownership of the underlying handle across
    // multiple RawHandleResource instances.
    struct HandleStateBase : Nonmovable
    {
        // Shio: To allow for in-place construction of non-default-constructible
        // handles, we use aligned_storage for raw memory and manage the
        // object's lifetime manually with a flag.
        raw_storage_t mHandleStorage;
        bool          mInitialized = false;

        HandleStateBase()
        {
            if constexpr(PerformDefaultConstructionOnInternalStorage)
            {
                // todo: should `::new` be used here instead?
                new(&handle()) raw_storage_t();
            }
        }

        virtual ~HandleStateBase() = default;

        auto & handle(this auto && self)
        {
            // `launder` is necessary to convert a pointer to the underlying
            // storage to the actual object stored. Otherwise, the access
            // behavior is undefined.
            /*
             * But this is only required when we are using `aligned_storage`
             * together.
             */
            if constexpr(is_using_raw_storage_block_v)
            {
                return *std::launder(
                    reinterpret_cast<std::conditional_t<
                        std::is_const_v<decltype(self)>, const raw_handle_t *,
                        raw_handle_t *
                    >>(&self.mHandleStorage)
                );
            }
            else
            {
                return self.mHandleStorage;
            }
        }
    };

    static_assert(
        std::has_virtual_destructor_v<HandleStateBase>,
        "HandleStateBase must has a virtual dtor otherwise HandleState will be "
        "sliced."
    );

    template <typename DestroyFunc>
    struct HandleState : HandleStateBase
    {
        DestroyFunc mDestroyer;

        explicit HandleState(DestroyFunc destroy_func)
            : mDestroyer(std::move(destroy_func))
        {
        }

        // Shio: The destructor is called automatically by the shared_ptr when
        // the last reference is released. It invokes the destroyer to clean
        // up the raw handle.
        ~HandleState() override
        {
            // Shio: The check on mInitialized ensures we only destroy a
            // fully constructed handle.
            if(this->mInitialized)
            {
                mDestroyer(this->handle());
                if constexpr(is_using_raw_storage_block_v)
                {
                    // For safety purpose, run the dtor of the payload because
                    // we are using a chunk of raw memory.
                    this->handle().~RawHandleT();
                }
                // Always reset destructed members to avoid unintentional
                // memory misuse.
                this->mInitialized = false;
            }
            // Shio: If init throws, mInitialized is false. If the user wants
            // cleanup anyway, we call the destroyer. This is risky as the
            // handle is partially constructed.
            else if constexpr(PerformCleanUpOnInitFuncThrows)
            {
                // Check whether we are during stack unwinding.
                // Because `this->mInitialized == false`, it means the
                // exception was caused by the init function.
                if(std::uncaught_exceptions() > 0)
                {
                    mDestroyer(this->handle());
                    // If the payload is not fully constructed, we cannot safely
                    // run its dtor on raw bytes because they may be garbage
                    // if the storage was not constructed.
                    // So paradoxically, we can only call the dtor when we are
                    // not using aligned storage.
                    /*
                    if constexpr(!is_using_raw_storage_block_v)
                    {
                        this->handle().~RawHandleT();
                    }
                    */
                    // todo currently for safety purposes we always ensure
                    //   the storage is default-constructed.
                    if constexpr(is_using_raw_storage_block_v)
                    {
                        this->handle().~RawHandleT();
                    }
                }
            }
        }
    };

    std::shared_ptr<HandleStateBase> mState;

    // Shio: Private constructor for delegation. Allocates state and places
    // the destroyer.
    template <typename DestroyFunc>
    explicit RawHandleResource(DestroyFunc destroy_func)
    // requires std::is_nothrow_invocable_v<DestroyFunc, raw_handle_t &>
    {
        // We cannot use `make_shared_for_overwrite` here. Because if the init
        // function throws an exception and `PerformCleanUpOnInitFuncThrows`
        // is true, and `mHandleStorage` is not default-initialized, the
        // `DestroyFunc` might be dealing with trash values, which could lead
        // to undefined behaviors. Therefore, to allow
        // `PerformCleanUpOnInitFuncThrows`, we must always use `make_shared`.
        if constexpr(PerformCleanUpOnInitFuncThrows)
        {
            mState = std::make_shared<HandleState<DestroyFunc>>(
                std::move(destroy_func)
            );
        }
        // Only when `PerformCleanUpOnInitFuncThrows` is not enabled, we can
        // safely use placement news.
        else
        {
            mState = std::make_shared_for_overwrite<HandleState<DestroyFunc>>();
            new(&mState->mDestroyer) DestroyFunc(std::move(destroy_func));
            mState->mInitialized = false;
        }
    }

protected:
    // Shio: Default constructor for derived classes that may want to
    // initialize later or not at all.
    RawHandleResource() = default;

public:
    /**
     * \brief Initializes the resource handle using the provided init function.
     * \details Shio: This is the primary and most straightforward constructor.
     *          It takes a function that produces a handle and a function that
     *          destroys it.
     * \tparam InitFunc A callable type for resource initialization.
     * \tparam DestroyFunc A callable type for resource destruction.
     * \param init_func A callable that returns a RawHandleT.
     * \param destroy_func A callable that takes a RawHandleT to clean it up.
     */
    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(const InitFunc & init_func, DestroyFunc destroy_func)
        requires std::
            is_convertible_v<std::invoke_result_t<InitFunc>, raw_handle_t>
        : RawHandleResource(std::move(destroy_func))
    {
        auto * state = static_cast<HandleState<DestroyFunc> *>(mState.get());
        new(&state->handle()) raw_handle_t(init_func());
        state->mInitialized = true;
    }

    /**
     * \brief Initializes the resource by constructing the handle in-place from
     *        a tuple of arguments.
     * \details Shio: This advanced constructor is for maximum performance. It
     *          takes an `InitFunc` that returns a tuple-like object (e.g.,
     *          `std::make_tuple(...)`). It then uses `std::apply` to perfectly
     *          forward these arguments to the `RawHandleT` constructor,
     *          building the handle directly in its final memory location.
     * \tparam InitFunc A callable that returns a tuple-like object containing
     *         the constructor arguments for `RawHandleT`.
     * \tparam DestroyFunc A callable for resource destruction.
     */
    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(const InitFunc & init_func, DestroyFunc destroy_func)
        // todo: write requirements for this ctor. the requirement should be
        //   that the return type of InitFunc can be
        : RawHandleResource(std::move(destroy_func))
    {
        auto * state = static_cast<HandleState<DestroyFunc> *>(mState.get());

        // todo: write a requirement for this ctor to make sure that the
        // init_func can return a tuple-like object that can be used to
        // initialize mHandleStorage.
        meta::reflection::tie_nonstatic_members(*state) = init_func();
        state->mInitialized                             = true;
    }

    /**
     * \brief (C++26) Initializes a C-style raw handle by providing references
     *        to its members to an initializer function.
     * \details Shio: This is a highly advanced constructor that uses C++26
     *          static reflection. It treats `RawHandleT` as a plain memory
     *          layout. It reflects on its members to find their types and
     *          offsets, then passes references to these raw memory locations
     *          to the `init_func`. The `init_func` is expected to populate the
     *          members directly. This is powerful for C-interop or when a
     *          handle is not aggregate-constructible but can be initialized
     *          member-by-member.
     * \param init_func A callable that takes references to each of
     *        `RawHandleT`'s members as arguments.
     * \param destroy_func
     */
    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(
        construct_by_member_refs_t,
        const InitFunc & init_func,
        DestroyFunc      destroy_func
    )
        : RawHandleResource(std::move(destroy_func))
    {
        // This is currently default-constructed.
        auto & states = mState->handle();

        // 1. We are going to call `init_func` with references to
        // `mHandleStorage`'s members. Are we going to make our life difficult?
        // No. We have things that can be reused.

        // Remember, depending on `is_using_raw_storage_block_v`, the values of
        // members in `mHandleStorage` might *not* be constructed, especially
        // when `make_shared_for_overwrite` is used. Thought that currently,
        // for safety reason, we always default-construct the states.
        // ... until we can make sure no memory leak would happen, by somehow
        // enforcing the `init_func` to actually initialize every member.
        // Use magic number checks maybe?

        // This should behave like `std::tie` and return a tuple of references
        // to the aggregate members.
        const auto member_refs =
            meta::reflection::tie_nonstatic_members(states);

        std::apply(init_func, member_refs);

        mState->mInitialized = true;
    }

    template <typename InitFunc, typename DestroyFunc>
    RawHandleResource(
        construct_by_ref_t, const InitFunc & init_func, DestroyFunc destroy_func
    )
        : RawHandleResource(std::move(destroy_func))
    {
        // This is currently default-constructed.
        auto & states = mState->handle();
        init_func(states);
        mState->mInitialized = true;
    }

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
        // the result.
        return self.template _TryGetRawHandleRef<false, false>().value().get();
    }

    /**
     * \brief (DANGEROUS) Retrieves a mutable reference to the raw handle.
     * \warning This function is intentionally restricted and dangerous. It
     *          provides a mutable reference to the underlying handle, breaking
     *          const-correctness guarantees. It should only be enabled and
     *          used when you are absolutely certain that mutating the handle
     *          is safe and will not violate the resource's invariants. Any use
     *          of this function is at your own risk.
     * \return A mutable reference to the raw handle.
     * \throws MissingManagedResource if the handle is not available.
     */
    handle_mutable_access_t GetRawHandleMutable(this auto && self)
        requires(!return_handle_by_copy_v && AllowMutableHandleAccess)
    {
        return self.template _TryGetRawHandleRef<false, true>().value().get();
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
     *          handle is missing.
     * \tparam NoExcept If true, returns an empty result on failure. If false,
     *         throws `MissingManagedResource`.
     * \return A `std::expected` containing a `std::reference_wrapper` to the
     *         handle if present, or an error state otherwise.
     */
    template <bool NoExcept, bool Mutable>
    auto _TryGetRawHandleRef(this auto && self) -> std::expected<
        std::reference_wrapper<
            std::conditional_t<Mutable, RawHandleT, const RawHandleT>
        >,
        ResourceErrorCodes
    >
    {
        if(self.mState && self.mState->mInitialized)
        {
            if constexpr(Mutable)
            {
                return std::ref(self.mState->handle());
            }
            else
            {
                // Shio: If the handle exists, wrap it in a const reference.
                // std::cref ensures we get a reference-to-const.
                return std::cref(self.mState->handle());
            }
        }

        // Shio: If the handle does not exist...
        if constexpr(NoExcept)
        {
            // Shio: ...return an error state.
            return std::unexpected(ResourceErrorCodes::ResourceDisappeared);
        }
        else
        {
            // Shio: ...or throw an exception if requested.
            throw MissingManagedResource(
                "The managed resource is not available."
            );
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
} // namespace usagi::runtime
