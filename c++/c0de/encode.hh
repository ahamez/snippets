#pragma once

#include <type_traits>
#include <utility>

#include "code.hh"

namespace codes {

/*------------------------------------------------------------------------------------------------*/

// Default way of applying a xor.
template <typename T>
inline
T
apply_xor(const T& lhs, const T& rhs)
{
  return lhs ^ rhs;
}

/*------------------------------------------------------------------------------------------------*/

template <typename... Ks>
struct apply_impl;

template <typename K>
struct apply_impl<K>
{
  template <std::size_t N>
  vector_data_type
  operator()(const input_type<N>& input, const std::size_t i)
  const noexcept
  {
    static_assert(K::which < N, "Invalid K source");
    return input[K::which][(K::index * quarter) + i];
  }
};

template <typename K, typename... Ks>
struct apply_impl<K, Ks...>
{
  template <std::size_t N>
  vector_data_type
  operator()(const input_type<N>& input, const std::size_t i)
  const noexcept
  {
    static_assert(K::which < N, "Invalid K source");
    return apply_xor(input[K::which][(K::index * quarter) + i], apply_impl<Ks...>{}(input, i));
  }
};

template <typename... Ks>
struct apply;

template <typename... Ks>
struct apply<krow<Ks...>>
{
  template <std::size_t N>
  void
  operator()(vector_type::iterator repair, const input_type<N>& input, const std::size_t base)
  const noexcept
  {
    for (auto i = 0ul; i < quarter; ++i)
    {
      *(repair + base + i) = apply_impl<Ks...>{}(input, i);
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

template <typename Code, std::size_t I>
struct encode_impl
{
  template <std::size_t N>
  void
  operator()(vector_type& repair, const input_type<N>& input)
  const noexcept
  {
    encode_impl<Code, I - 1>{}(repair, input);
    apply<get_row_t<I, Code>>{}(begin(repair), input,  (I * quarter));
  }
};

template <typename Code>
struct encode_impl<Code, 0>
{
  template <std::size_t N>
  void
  operator()(vector_type& repair, const input_type<N>& input)
  const noexcept
  {
    apply<get_row_t<0, Code>>{}(begin(repair), input,  0 * quarter);
  }
};

template <typename Code, std::size_t N>
inline
void
encode(vector_type& repair, const input_type<N>& input)
noexcept
{
  static_assert(Code::size == 4, "TODO: handle number of rows != 4");
  static_assert(Code::size == N, "Mismatching number of rows and input packets");
  encode_impl<Code, N - 1>{}(repair, input);
}

/*------------------------------------------------------------------------------------------------*/

} // namespace codes
