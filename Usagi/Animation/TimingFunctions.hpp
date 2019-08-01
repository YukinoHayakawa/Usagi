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
extern const TimingFunction IN_OUT_SINE;
extern const TimingFunction OUT_SINE;

TimingFunction get(const std::string &name);
}
}
