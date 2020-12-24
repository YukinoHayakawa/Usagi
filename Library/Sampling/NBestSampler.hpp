#pragma once

#include <vector>

#include "ReservoirSampler.hpp"

namespace usagi
{
template <
    typename T,
    typename Comparator = std::less,
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

        if(best.size() < sample_size)
        {
            auto &r = best.emplace_back();
            r.sample_size = 1;
            r.visit(rng, element);
        }
        else
        {
            for(auto i = best.begin(); i != best.end(); ++i)
            {
                // todo use spaceship operator?
                // same value. break tie uniformly using reservoir sampling.
                if(projection(element) == projection(*i))
                {
                    i->visit(rng, element);
                    break;
                }
                // found better value. make room for it.
                if(comparator(projection(element), projection(*i)))
                {
                    // pop the largest one
                    best.pop_back();
                    // insert new one at current position
                    auto iter = best.insert(i);
                    iter->sample_size = 1;
                    iter->visit(rng, element);
                    break;
                }
            }
        }
    }
};
}
