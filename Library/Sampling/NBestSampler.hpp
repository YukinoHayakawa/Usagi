#pragma once

#include <vector>

#include "ReservoirSampler.hpp"

namespace usagi
{
template <
    typename T,
    typename Comparator,
    typename Projection = std::identity,
    typename Container = std::vector<ReservoirSampler<std::vector<T>>>
>
struct NBestSampler
{
    std::size_t sample_size = 1;
    Container best;
    Comparator comparator { };
    Projection projection { };

    // https://www.geeksforgeeks.org/find-the-largest-three-elements-in-an-array/
    void visit(auto &&rng, auto &&element)
    {
        assert(sample_size);
        // bug this sampler incorrectly draw 1 sample from each of top N best bins rather than from all elements
        if(best.size() < sample_size)
        {
            best.emplace_back();
            auto &r = best.back();
            r.sample_size = 1;
            r.visit(rng, element);
        }
        else
        {
            for(auto i = best.begin(); i != best.end(); ++i)
            {
                // todo use spaceship operator?
                // same value. break tie uniformly using reservoir sampling.
                if(projection(element) == projection(i->samples[0]))
                {
                    i->visit(rng, element);
                    break;
                }
                // found better value. make room for it.
                if(comparator(projection(element), projection(i->samples[0])))
                {
                    // pop the largest one
                    best.pop_back();
                    // insert new one at current position
                    auto iter = best.emplace(i);
                    iter->sample_size = 1;
                    iter->visit(rng, element);
                    break;
                }
            }
        }
    }

    void visit_range(auto &&rng, auto &&range)
    {
        for(auto &&e : range)
        {
            visit(rng, e);
        }
    }
};
}
