#pragma once

#include "accum_traits.hh"

template <
  typename T,
  typename AT = AccumulationTraits<T>
>
struct Accum
{
  static
  typename AT::AccT
  accum(T const* beg, T const* end)
  {
    typename AT::AccT total = AT::zero();

    while(beg != end)
    {
      total += *beg;
      ++beg;
    }

    return total;
  }
};
