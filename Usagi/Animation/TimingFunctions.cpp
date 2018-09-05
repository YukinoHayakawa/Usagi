#include "TimingFunctions.hpp"

namespace usagi::timing_functions
{
const TimingFunction LINEAR = [](const auto t) {
    return t;
};
}
