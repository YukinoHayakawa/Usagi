#pragma once

namespace usagi
{
class Nonmovable
{
public:
    Nonmovable() = default;
    Nonmovable(Nonmovable &&other) noexcept = delete;
    Nonmovable & operator=(Nonmovable &&other) noexcept = delete;
};
}
