#pragma once

namespace usagi::runtime
{
/*
 * Shio: As requested, this enum is defined but not used in the return type
 * of the serialization function. The error handling is done by embedding
 * error information within the resulting JSON/other object itself.
 */
enum class SerializationErrorCodes
{
    MaxRecursionDepthReached,
    TypeNotMatch,
};
} // namespace usagi::runtime
