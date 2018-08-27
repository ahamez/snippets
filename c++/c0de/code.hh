#pragma once

#include <array>
#include <cstdint>

#include "meta.hh"

namespace codes {
  
/*------------------------------------------------------------------------------------------------*/

// TODO try different alignment
// TODO use a structure to pack these configuration parameters

static constexpr auto vector_bytes = 1024ul;
// static constexpr auto vector_bytes = 4096ul;
static_assert(is_power_of_two(vector_bytes), "vector_bytes should be a power of 2");

using vector_data_type = char;
// using vector_data_type = std::uint_fast64_t;
// using vec = char __attribute__ ((vector_size(128)));
// using vector_data_type = vec;
static constexpr auto vector_size = vector_bytes / sizeof(vector_data_type);
static constexpr auto quarter = vector_size / 4;

using vector_type = std::array<vector_data_type, vector_size>;

template <std::size_t N>
using input_type = std::array<vector_type::const_iterator, N>;  

/*------------------------------------------------------------------------------------------------*/

template <typename... Brows>
struct coefficient
{
  static constexpr auto size = sizeof...(Brows);

  template <typename Brow>
  struct is_square_impl
  {
    static constexpr auto value = size == Brow::size;
  };

  static constexpr auto is_square = for_all<is_square_impl, Brows...>::value;
};

/*------------------------------------------------------------------------------------------------*/

// A row of binary values in a coefficient.
template <bool... Values>
struct brow
{
  static constexpr auto size = sizeof...(Values);
};

/*------------------------------------------------------------------------------------------------*/

// A code is mapping describing row by row to construct a repair packet.
// For instance, the following code describes a simple xor:
//   using simple_xor = code< krow< k<0,0>, k<1,0>, k<2,0>, k<3,0> > // R1[0]
//                          , krow< k<0,1>, k<1,1>, k<2,1>, k<3,1> > // R1[1]
//                          , krow< k<0,2>, k<1,2>, k<2,2>, k<3,2> > // R1[2]
//                          , krow< k<0,3>, k<1,3>, k<2,3>, k<3,3> > // R1[3]
//                          >;
//   R1[0] = k0[0] xor k1[0] xor k2[0] xor k3[0]
//   R2[0] = k0[1] xor k1[1] xor k2[1] xor k3[1]
//   etc.
template <typename... Rows>
struct code
{
  static constexpr auto size = sizeof...(Rows);
};

/*------------------------------------------------------------------------------------------------*/

// Describe a k-row in a code.
template <typename... Xs>
struct krow
{
  static constexpr auto size = sizeof...(Xs);
};

/*------------------------------------------------------------------------------------------------*/

template <std::size_t K, std::size_t I>
struct k
{
  static constexpr auto which = K;
  static constexpr auto index = I;
};

/*------------------------------------------------------------------------------------------------*/

template <std::size_t, typename...>
struct get_row_impl;

template <typename R, typename... Rs>
struct get_row_impl<0, R, Rs...>
{
  using type = R;
};

template <std::size_t Index, typename R, typename... Rs>
struct get_row_impl<Index, R, Rs...>
{
  static_assert(Index < sizeof...(Rs) + 1 /* + 1 for R */, "Index too large for get_row");
  using type = typename get_row_impl<Index - 1, Rs...>::type;
};

template <std::size_t , typename...>
struct get_row;

template <std::size_t Index, typename... Rows>
struct get_row<Index, code<Rows...>>
{
  using type = typename get_row_impl<Index, Rows...>::type;
};

template <std::size_t Index, typename Code>
using get_row_t = typename get_row<Index, Code>::type;

/*------------------------------------------------------------------------------------------------*/

} // namespace codes
