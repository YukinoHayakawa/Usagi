#include "TimingFunctions.hpp"

#include <map>

#include <Usagi/Core/Logging.hpp>

namespace usagi::timing_functions
{
const TimingFunction LINEAR = [](const auto t) {
    return t;
};

TimingFunction get(const std::string &name)
{
    static const std::map<std::string, TimingFunction> EASINGS =
    {
        { "linear", LINEAR },
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
