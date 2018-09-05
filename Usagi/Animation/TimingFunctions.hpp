#pragma once

#include <functional>

namespace usagi
{
/**
 * \brief Controls how animation progresses.
 */
using TimingFunction = std::function<double(double t)>;

// ref:
// https://easings.net/
// https://matthewlein.com/tools/ceaser
namespace timing_functions
{
extern const TimingFunction LINEAR;
}
}
