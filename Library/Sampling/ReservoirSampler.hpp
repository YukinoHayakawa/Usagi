#pragma once

#include <cassert>
#include <cstddef>
#include <random>

namespace usagi
{
// https://en.wikipedia.org/wiki/Reservoir_sampling#Simple_algorithm
// https://web.ma.utexas.edu/users/parker/sampling/repl.htm
template <typename Container>
struct ReservoirSampler
{
    std::size_t num_visited = 0;
    std::size_t sample_size = 0;

    Container samples;

    void visit(auto &&rng, auto &&element)
    {
        assert(sample_size);

        // fill the reservoir array
        if(num_visited < sample_size)
        {
            samples.push_back(element);
        }
        // replace elements with gradually decreasing probability
        else
        {
            const std::uniform_int_distribution<std::size_t> dist {
                0, num_visited
            };
            if(const auto idx = dist(rng); idx < sample_size)
            {
                samples[idx] = element;
            }
        }
        ++num_visited;
    }

    void visit_range(auto &&rng, auto &&range)
    {
        for(auto &&e : range)
        {
            visit(rng, e);
        }
    }

    void reset()
    {
        num_visited = 0;
        samples.clear();
    }
};
}
