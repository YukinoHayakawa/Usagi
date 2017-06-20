#pragma once

namespace yuki
{

template <typename T>
class Criteria
{
public:
    virtual ~Criteria()
    {
    }

    virtual bool isEligible(const T &target) = 0;
};

}
