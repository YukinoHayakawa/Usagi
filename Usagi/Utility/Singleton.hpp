﻿#pragma once

#include <stdexcept>

#include <Usagi/Core/Exception.hpp>

#include "Noncopyable.hpp"

namespace usagi
{
template <typename T>
class Singleton : Noncopyable
{
    inline static T *mInstance = nullptr;

public:
    Singleton()
    {
        if(mInstance)
            USAGI_THROW(std::runtime_error(
                "Only one instance of T can be created!"
            ));

        mInstance = static_cast<T*>(this);
    }

    virtual ~Singleton()
    {
        mInstance = nullptr;
    }

    static T * instance()
    {
        return mInstance;
    }
};
}
