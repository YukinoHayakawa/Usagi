#pragma once

#include <expected>

#include <Usagi/Library/Enums/Bitwise.hpp>

namespace usagi::runtime::errors
{
// todo: is it possible to implement a safer container to ensure segments don't
//   accidentally get cross each other?
enum class SystemErrorCodes : std::uint32_t
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

    ErrorCodeMask = 0x0000'ffff,

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

    // Memory & Resource
    OutOfMemory           = ResourceError | _NEXT_OF(UnexpectedCodePath),
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
};

#undef _NEXT_OF
} // namespace usagi::runtime::errors

namespace usagi
{
template <>
struct EnableBitMaskOperators<runtime::errors::SystemErrorCodes>
    : std::true_type
{
};

template <typename T>
using ExpectedSyscallValue =
    std::expected<T, runtime::errors::SystemErrorCodes>;
} // namespace usagi

namespace usagi::runtime::errors
{
// --- Evaluation Helpers ---

constexpr auto domains_of(const SystemErrorCodes code) noexcept
{
    return code & SystemErrorCodes::DomainMask;
}

constexpr bool has_domain(
    const SystemErrorCodes code, const SystemErrorCodes domain) noexcept
{
    return has_any_of(domains_of(code), domain);
}

constexpr auto severity_flags_of(const SystemErrorCodes code) noexcept
{
    return code & SystemErrorCodes::SeverityMask;
}

constexpr bool has_severity_flag(
    const SystemErrorCodes code, const SystemErrorCodes severity) noexcept
{
    return has_any_of(severity_flags_of(code), severity);
}

// Shio:
// The following semantic queries evaluate the *effective* threat level.
// Since multiple severity flags can be combined, these functions test the
// presence of a flag while ensuring no *higher* threat flag overrides it.
// Fatal overrides everything.

constexpr auto top_severity_flag_of(const SystemErrorCodes code) noexcept
{
    return most_significant_flag(severity_flags_of(code));
}

constexpr bool is_fatal(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::SeverityFatal;
}

constexpr bool is_logic_error(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::SeverityLogicError;
}

constexpr bool is_hard_fault(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::SeverityHardFault;
}

constexpr bool is_transient(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::SeverityTransient;
}

constexpr bool is_ignorable(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::SeverityIgnorable;
}

constexpr bool is_undecidable(const SystemErrorCodes code) noexcept
{
    return top_severity_flag_of(code) == SystemErrorCodes::NoSeverity;
}

constexpr bool is_elevated(const SystemErrorCodes code) noexcept
{
    return num_flags(severity_flags_of(code)) > 1;
}

/**
 * @brief Injects or adds a severity flag to an existing error code.
 */
constexpr SystemErrorCodes add_severity(
    const SystemErrorCodes code, const SystemErrorCodes severity) noexcept
{
    return code | severity;
}

namespace details::static_tests
{
static_assert(SystemErrorCodes::DomainMask == 0x00ff'0000u);
static_assert(SystemErrorCodes::SeverityMask == 0xff00'0000u);

static_assert(domains_of(SystemErrorCodes::FileNotFound) ==
    (SystemErrorCodes::DeviceIOError | SystemErrorCodes::ParameterError));
static_assert(has_domain(
    SystemErrorCodes::FileNotFound, SystemErrorCodes::DeviceIOError));
static_assert(has_domain(
    SystemErrorCodes::FileNotFound, SystemErrorCodes::ParameterError));
static_assert(!has_domain(
    SystemErrorCodes::FileNotFound, SystemErrorCodes::SecurityError));

static_assert(
    is_undecidable(SystemErrorCodes::FileNotFound)); // Baseline is undecidable
static_assert(is_fatal(add_severity(
    SystemErrorCodes::FileNotFound, SystemErrorCodes::SeverityFatal)));
static_assert(is_transient(add_severity(
    SystemErrorCodes::FileNotFound, SystemErrorCodes::SeverityTransient)));

constexpr SystemErrorCodes escalated_error =
    add_severity(add_severity(SystemErrorCodes::FileNotFound,
                     SystemErrorCodes::SeverityTransient),
        SystemErrorCodes::SeverityFatal);

static_assert(is_fatal(escalated_error));
static_assert(!is_transient(escalated_error)); // Fatal overrides transient
static_assert(is_elevated(escalated_error));
} // namespace details::static_tests
} // namespace usagi::runtime::errors
