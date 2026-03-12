#pragma once

#include <Usagi/Runtime/Errors/SystemErrorCodes.hpp>

namespace usagi::platforms::runtime
{
// todo: maybe we are not in Windows. better name?
extern "C" usagi::runtime::errors::SystemErrorCodes execute_seh_trampoline(
    void (*invoker)(void *), void *context);
} // namespace usagi::platforms::runtime
