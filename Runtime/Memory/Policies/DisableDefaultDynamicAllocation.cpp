// Disable the default heap allocations so we can have better control over
// memory consumption.
//
// https://stackoverflow.com/questions/18365804/is-it-possible-to-completely-disable-the-default-c-new-operator

#include <cstddef>

extern void * do_not_use_default_heap_allocation();

#ifdef _MSC_VER
#pragma warning(push)
// 'operator new': non-member operator new or delete functions may not be declared inline
#pragma warning(disable: 4595)
#endif

inline void * operator new(std::size_t)
{
    return do_not_use_default_heap_allocation();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

