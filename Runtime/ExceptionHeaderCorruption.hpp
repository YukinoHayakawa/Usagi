#pragma once

#include <stdexcept>

namespace usagi
{
class ExceptionHeaderCorruption : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
}
