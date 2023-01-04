#pragma once

namespace usagi
{
template <typename T>
concept StructDeclaresReadAccess = requires { typename T::ReadAccess; };

template <typename T>
concept StructDeclaresWriteAccess = requires { typename T::WriteAccess; };
}
