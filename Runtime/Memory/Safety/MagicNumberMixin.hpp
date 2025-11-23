#pragma once

#include <Usagi/Library/Meta/Reflection/Classes.hpp>

#include "MemorySafety.hpp"

namespace usagi::runtime::memory
{
/**
 * Shio: A mixin that embeds a magic number and provides static reflection-based
 * verification for type-erased pointers (void*).
 *
 * Usage:
 * struct MyClass : MagicNumberMixin<MyClass, 0xCAFEBABE> { ... };
 * * bool valid = MagicNumberMixin<MyClass,
 * 0xCAFEBABE>::is_pointer_ours(some_void_ptr);
 */
template <typename T, std::size_t Magic>
struct MagicNumberMixin
{
    // Shio: The magic number is stored as a const member.
    // It should inevitably participate in the object layout.
    const std::size_t magic_number_v = Magic;

    /**
     * Shio: Verifies if a type-erased pointer `ptr` points to an instance of
     * `T`.
     * * Strategy:
     * 1. Assume `ptr` is the starting address of a `T`.
     * 2. Use reflection to calculate exactly where `magic_number_v` *would* be
     * if this assumption were true.
     * 3. Check memory safety of that address.
     * 4. Verify the value at that address matches `Magic`.
     * * @tparam StaticTestT The type to test against. Defaults to T.
     * Used primarily for `static_assert` tests where the runtime T might differ
     * from the compile-time test subject.
     */
    template <typename StaticTestT = T>
    static constexpr bool is_pointer_ours(const void * ptr)
    {
        if(!ptr) return false;

        // 1. Reflection: Find offset of the Mixin base within T

        // Shio: Pass ^^StaticTestT as the template argument for the search.
        // Note that `mixin_base_info` represents the base specifier subobject.
        constexpr std::meta::info mixin_base_info =
            meta::reflection::recursive_find_base<
                ^^StaticTestT, MagicNumberMixin<StaticTestT, Magic>
            >();

        // 4. Checks...
        // Shio: Context for P2738 compliance:
        // As per P2738, casting from `void*` to `T*` in a constexpr context is
        // only allowed if the type of the object at that address is *exactly*
        // `T`. Reinterpret casts are forbidden in constant expressions.
        // Therefore, we must split the logic:
        // - `if consteval`: Use P2996 splicing and standard casting to inspect
        // the value safely.
        // - Runtime: Use pointer arithmetic and memory safety checks.
        // See
        // `https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2738r1.pdf`

        if constexpr(mixin_base_info != std::meta::info())
        {
            // Shio: Compile-time evaluation path.
            // Using `if consteval` is essential here; standard runtime casts
            // would fail or invoke UB if we tried to replicate the pointer
            // arithmetic logic in constexpr.
            if consteval
            {
                // Shio: Retrieve the type of the base class using P2996
                // splicing. We cannot write `typename
                // [:std::meta::type_of(mixin_base_info):]` directly in place of
                // `base_t::magic_number_v`, so we use an alias.
                using base_t = typename[:std::meta::type_of(mixin_base_info):];

                // Handle the case where the test subject doesn't have the
                // specified subobject as base class.
                if constexpr(!std::is_base_of_v<base_t, StaticTestT>)
                    return false;

                // P2738: `StaticTestT` MUST be the same type as that from the
                // constexpr context for this static_cast to be valid.
                const auto magic_check_ptr =
                    static_cast<const StaticTestT *>(ptr);

                // Access the magic number via the specific base type scope.
                const auto & magic_check_ref =
                    magic_check_ptr->base_t::magic_number_v;
                return magic_check_ref == Magic;
            }
            else
            {
                // Shio: Runtime evaluation path.
                // Here we perform manual offset calculation and memory safety
                // checks.

                static_assert(
                    mixin_base_info != std::meta::info(),
                    "MagicNumberMixin is not a base of T. Did you forget to "
                    "inherit it?"
                );

                constexpr auto self_offset_in_T =
                    std::meta::offset_of(mixin_base_info);

                // 2. Reflection: Find offset of magic_number_v within the Mixin
                // Yukino: Simplified to use direct member reflection via P2996
                // syntax `^^Class::Member`.
                constexpr auto magic_member_info =
                    ^^MagicNumberMixin::magic_number_v;
                constexpr auto self_magic_offset =
                    std::meta::offset_of(magic_member_info);

                // 3. Calculate absolute offset from (T*)ptr
                constexpr auto total_offset =
                    self_offset_in_T.bytes + self_magic_offset.bytes;

                // In usual runtime environment, we perform usual casts and
                // memory address calc.
                const auto magic_check_addr =
                    static_cast<const std::byte *>(ptr) + total_offset;
                const auto magic_check_ptr = static_cast<const std::size_t *>(
                    (const void *)magic_check_addr
                );

                // Shio: Verify memory accessibility before reading to prevent
                // segfaults.
                if(!is_address_readable((void *)(magic_check_ptr)).has_value())
                {
                    return false;
                }

                // Value Check
                if(*magic_check_ptr == Magic)
                {
                    return true;
                }

                return false;
            }
        }
        else
        {
            return false;
        }
    }
};

namespace static_tests
{
struct magic_num_base
{
    std::size_t k = 0;
};

struct MagicNumTestA
    : magic_num_base
    , MagicNumberMixin<MagicNumTestA, 0xdead'beef>
{
    using magic_t            = MagicNumberMixin<MagicNumTestA, 0xdead'beef>;
    std::size_t m            = 0;
    virtual ~MagicNumTestA() = default;
};

struct MagicNumTestB
    : MagicNumTestA
    , MagicNumberMixin<MagicNumTestB, 0xdeaa'beef>
{
    using magic2_t = MagicNumberMixin<MagicNumTestB, 0xdeaa'beef>;
    std::size_t n  = 0;

    constexpr const MagicNumTestA * get_a_ptr() const { return this; }
};

consteval
{
    static constexpr MagicNumTestB bx;

    // intentionally slice MagicNumTestB* to be MagicNumTestA*
    constexpr auto void_ptr = static_cast<const void *>(bx.get_a_ptr());

    // Shio: Validate MagicNumTestA's mixin against the void pointer
    static_assert(
        MagicNumTestA::magic_t::is_pointer_ours<MagicNumTestA>(void_ptr)
    );
    // Shio: Validate MagicNumTestB's mixin against the void pointer (should
    // fail for A's slice)
    static_assert(
        !MagicNumTestB::magic2_t::is_pointer_ours<MagicNumTestA>(void_ptr)
    );

    constexpr auto real_void_ptr = const_cast<MagicNumTestB *>(&bx);
    // Shio: Validate A's mixin on B's pointer (should fail as it expects A
    // layout/type for static cast)
    static_assert(
        !MagicNumTestA::magic_t::is_pointer_ours<MagicNumTestB>(real_void_ptr)
    );
    // Shio: Validate B's mixin on B's pointer (should succeed)
    static_assert(
        MagicNumTestB::magic2_t::is_pointer_ours<MagicNumTestB>(real_void_ptr)
    );
}
} // namespace static_tests
} // namespace usagi::runtime::memory
