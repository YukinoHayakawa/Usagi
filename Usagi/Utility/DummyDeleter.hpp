#pragma once

namespace usagi
{
struct DummyDeleter
{
    void operator()(void *ptr) const { }
};
}
