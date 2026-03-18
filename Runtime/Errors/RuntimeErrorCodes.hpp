#pragma once

#include <expected>

#include <Usagi/Library/Enums/Bitwise.hpp>

namespace usagi::runtime::errors
{
// todo: is it possible to implement a safer container to ensure segments don't
//   accidentally get cross each other?
// todo: e.g. CombinedFlags<...>?
enum class RuntimeErrorCodes : std::uint32_t
{
    Success = 0,
    NoError = Success,

    SegmentMask = 0xFFu,

    // --- Domain Category Flags (Bits 16-23) ---
    // Represents *where* or *what kind* of system failed.

    NoDomain    = 0,
    DomainShift = 16,
    DomainMask  = SegmentMask << DomainShift,

    // Semantic or state machine failures
    StateError         = 1 << (0 + DomainShift),
    // Exhaustion of measurable resources
    ResourceError      = 1 << (1 + DomainShift),
    // Violations of trust, access, or rights
    SecurityError      = 1 << (2 + DomainShift),
    // Caller logic errors
    ParameterError     = 1 << (3 + DomainShift),
    // Hardware or communication failures
    DeviceIOError      = 1 << (4 + DomainShift),
    // Corruption or validation failures
    DataIntegrityError = 1 << (5 + DomainShift),

    // --- Severity & Threat Flags (Bits 24-31) ---
    // Represents *how dangerous* the error is. These flags are combinable.
    // Evaluation functions should check the highest order bits first to
    // determine the true threat level if multiple flags are set.

    NoSeverity    = 0,
    SeverityShift = 24,
    SeverityMask  = SegmentMask << SeverityShift,

    // The severity is currently unknown and must be evaluated by a higher
    // layer.
    SeverityUndecidable = 0,
    // No state mutation occurred. Safe to drop entirely. (e.g., NoOp)
    SeverityIgnorable   = 1 << (0 + SeverityShift),
    // Resource constrained or busy. Recoverable upon retry or waiting.
    SeverityTransient   = 1 << (1 + SeverityShift),
    // Unavoidable failure under current parameters (e.g., AccessDenied). Must
    // be handled by changing approach.
    SeverityHardFault   = 1 << (2 + SeverityShift),
    // API contract violation (e.g., OutOfBounds). Points to a bug in the
    // calling code.
    SeverityLogicError  = 1 << (3 + SeverityShift),
    // Unrecoverable systemic failure. The engine must attempt a clean shutdown.
    SeverityFatal       = 1 << (4 + SeverityShift),

    // --- Specific Errors (Bits 0-15) + Domain ---
    // Note: Severity flags are deliberately NOT baked into these definitions.
    // They should be bitwise-OR'd dynamically based on the execution context.

    ErrorCodeMask = 0x0000'FFFFu,

#define _NEXT_OF(prev_error) ((prev_error & ErrorCodeMask) + 1)

    // General Fallbacks
    UnknownError             = 1,
    // The OS lacks this feature entirely.
    NotImplementedByPlatform = StateError | _NEXT_OF(UnknownError),
    // Usagi hasn't implemented this path yet.
    NotImplementedByEngine   = StateError | _NEXT_OF(NotImplementedByPlatform),

    // State & Capability
    NoOp               = StateError | _NEXT_OF(NotImplementedByEngine),
    NotSupported       = StateError | _NEXT_OF(NoOp),
    UnreachableCode    = StateError | _NEXT_OF(NotSupported),
    UnexpectedCodePath = StateError | _NEXT_OF(UnreachableCode),
    // Queued async action.
    PendingAsyncAction = StateError | _NEXT_OF(UnexpectedCodePath),
    // Async action being processed.
    ActiveAsyncAction  = StateError | _NEXT_OF(PendingAsyncAction),

    // Memory & Resource
    OutOfMemory           = ResourceError | _NEXT_OF(ActiveAsyncAction),
    AddressSpaceExhausted = ResourceError | _NEXT_OF(OutOfMemory),
    QuotaExceeded         = ResourceError | _NEXT_OF(AddressSpaceExhausted),

    // Security & Access
    AccessDenied = SecurityError | _NEXT_OF(QuotaExceeded),
    FileInUse    = SecurityError | StateError | _NEXT_OF(AccessDenied),

    // Parameters & Bounds
    InvalidParameter = ParameterError | _NEXT_OF(FileInUse),
    AlignmentError   = ParameterError | _NEXT_OF(InvalidParameter),
    OutOfBounds      = ParameterError | _NEXT_OF(AlignmentError),
    RegionOverlap    = ParameterError | _NEXT_OF(OutOfBounds),

    // Device I/O & Integrity
    DeviceDisconnected = DeviceIOError | StateError | _NEXT_OF(RegionOverlap),
    FileNotFound =
        DeviceIOError | ParameterError | _NEXT_OF(DeviceDisconnected),
    PathTooLong    = DeviceIOError | ParameterError | _NEXT_OF(FileNotFound),
    DiskFull       = DeviceIOError | ResourceError | _NEXT_OF(PathTooLong),
    DataCorruption = DataIntegrityError | DeviceIOError | _NEXT_OF(DiskFull),

    NUM_ERROR_CODES = _NEXT_OF(DataCorruption),
};
// todo: bug -  it's weird that if I put `_NEXT_OF` inside RuntimeErrorCodes
//   clang-format will add a trailing comma for it??
#undef _NEXT_OF
} // namespace usagi::runtime::errors

namespace usagi
{
template <>
struct EnableBitMaskOperators<runtime::errors::RuntimeErrorCodes>
    : std::true_type
{
};

template <typename T>
using ExpectedRuntimeValue =
    std::expected<T, runtime::errors::RuntimeErrorCodes>;
} // namespace usagi

namespace usagi::runtime::errors
{
// --- Evaluation Helpers ---

constexpr auto domains_of(const RuntimeErrorCodes code) noexcept
{
    return code & RuntimeErrorCodes::DomainMask;
}

constexpr bool has_domain(
    const RuntimeErrorCodes code, const RuntimeErrorCodes domain) noexcept
{
    return has_any_of(domains_of(code), domain);
}

constexpr auto severity_flags_of(const RuntimeErrorCodes code) noexcept
{
    return code & RuntimeErrorCodes::SeverityMask;
}

constexpr bool has_severity_flag(
    const RuntimeErrorCodes code, const RuntimeErrorCodes severity) noexcept
{
    return has_any_of(severity_flags_of(code), severity);
}

// Shio:
// The following semantic queries evaluate the *effective* threat level.
// Since multiple severity flags can be combined, these functions test the
// presence of a flag while ensuring no *higher* threat flag overrides it.
// Fatal overrides everything.

constexpr auto top_severity_flag_of(const RuntimeErrorCodes code) noexcept
{
    return most_significant_flag(severity_flags_of(code));
}

constexpr bool is_fatal(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::SeverityFatal;
}

constexpr bool is_logic_error(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::SeverityLogicError;
}

constexpr bool is_hard_fault(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::SeverityHardFault;
}

constexpr bool is_transient(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::SeverityTransient;
}

constexpr bool is_ignorable(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::SeverityIgnorable;
}

constexpr bool is_undecidable(const RuntimeErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == RuntimeErrorCodes::NoSeverity;
}

constexpr bool is_elevated(const RuntimeErrorCodes code) noexcept
{
    return num_flags(severity_flags_of(code)) > 1;
}

/**
 * @brief Injects or adds a severity flag to an existing error code.
 */
constexpr RuntimeErrorCodes add_severity(
    const RuntimeErrorCodes code, const RuntimeErrorCodes severity) noexcept
{
    return code | severity;
}

namespace details::static_tests
{
static_assert(RuntimeErrorCodes::DomainMask == 0x00FF'0000u);
static_assert(RuntimeErrorCodes::SeverityMask == 0xFF00'0000u);

static_assert(
    domains_of(RuntimeErrorCodes::FileNotFound) ==
    (RuntimeErrorCodes::DeviceIOError | RuntimeErrorCodes::ParameterError));
static_assert(has_domain(
    RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::DeviceIOError));
static_assert(has_domain(
    RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::ParameterError));
static_assert(!has_domain(
    RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::SecurityError));

static_assert(
    is_undecidable(RuntimeErrorCodes::FileNotFound)); // Baseline is undecidable
static_assert(is_fatal(add_severity(
    RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::SeverityFatal)));
static_assert(is_transient(add_severity(
    RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::SeverityTransient)));

constexpr RuntimeErrorCodes escalated_error = add_severity(
    add_severity(
        RuntimeErrorCodes::FileNotFound, RuntimeErrorCodes::SeverityTransient),
    RuntimeErrorCodes::SeverityFatal);

static_assert(is_fatal(escalated_error));
// Fatal overrides transient
static_assert(!is_transient(escalated_error));
static_assert(is_elevated(escalated_error));
} // namespace details::static_tests
} // namespace usagi::runtime::errors
