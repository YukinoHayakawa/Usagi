#pragma once

#include <boost/math/constants/constants.hpp>

namespace yuki
{

class Angle
{
    float mValue;

public:
    Angle(float value)
        : mValue { value }
    {
    }

    float value() const
    {
        return mValue;
    }

    operator float() const
    {
        return value();
    }
};

class Radians : public Angle
{
public:
    using Angle::Angle;
};

class Degrees : public Angle
{
public:
    using Angle::Angle;

    Radians toRadians() const
    {
        return Radians(value() * boost::math::constants::pi<float>() / 180.f);
    }
};

}
