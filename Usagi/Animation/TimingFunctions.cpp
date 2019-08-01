#include "TimingFunctions.hpp"

#include <map>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Math/Bezier.hpp>
#include <Usagi/Utility/Functional.hpp>

namespace usagi::timing_functions
{
const TimingFunction LINEAR = [](const auto t) {
    return t;
};

// Bezier curve based ease functions
// https://www.w3schools.com/cssref/func_cubic-bezier.asp

const TimingFunction IN_OUT_SINE = partial_apply(
    &cssCubicBezier<double>, 0.445, 0.05, 0.55, 0.95
);

const TimingFunction OUT_SINE = partial_apply(
    &cssCubicBezier<double>, 0.39, 0.575, 0.565, 1
);

TimingFunction get(const std::string &name)
{
    static const std::map<std::string, TimingFunction> EASINGS =
    {
        { "linear", LINEAR },
        { "inOutSine", IN_OUT_SINE },
        { "outSine", OUT_SINE },
    };
    const auto i = EASINGS.find(name);
    if(i == EASINGS.end())
    {
        LOG(error, "No such easing function: {}", name);
        throw std::runtime_error("Non-existing key");
    }
    return i->second;
}
}
