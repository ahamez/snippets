#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <iterator>
#include <iostream>
#include <initializer_list>
#include <typeinfo>

/* ---------------------------------------------------------------------------------------------- */

template <typename Op, typename Lhs, typename Rhs>
struct array_exp
{
  static_assert(std::decay_t<Lhs>::dimension == std::decay_t<Rhs>::dimension, "");
  static constexpr auto dimension = std::decay_t<Lhs>::dimension;

  Lhs lhs;
  Rhs rhs;

  array_exp() = delete;

  array_exp(Lhs l, Rhs r)
    : lhs(std::forward<Lhs>(l)), rhs(std::forward<Rhs>(r))
  {}

  array_exp(const array_exp&) = delete;
  array_exp& operator=(const array_exp&) = delete;

  array_exp(array_exp&&) = default;
  array_exp& operator=(array_exp&&) = default;

  template <typename Rhs_>
  auto
  operator+(Rhs_&& rhs)
  noexcept
  -> array_exp<std::plus<>, const array_exp&, decltype(rhs)>
  {
    return {*this, std::forward<Rhs_>(rhs)};
  }

  template <typename Rhs_>
  auto
  operator*(Rhs_&& rhs)
  noexcept
  -> array_exp<std::multiplies<>, const array_exp&, decltype(rhs)>
  {
    return {*this, std::forward<Rhs_>(rhs)};
  }

  auto
  operator[](std::size_t n)
  const noexcept
  -> decltype(Op{}(lhs[n], rhs[n]))
  {
    return Op{}(lhs[n], rhs[n]);
  }
};

/* ---------------------------------------------------------------------------------------------- */

template <std::size_t N>
struct array
{
  using impl_type = std::array<int, N>;
  using value_type = typename impl_type::value_type;
  static constexpr auto dimension = N;

  impl_type impl;

  array(std::initializer_list<int> il)
  noexcept
    : impl{}
  {
    assert(il.size() == dimension);
    std::copy(begin(il), end(il), begin(impl));
  }

  array() = delete;
  array(const array& other) = default;
  array(array&&) = default;

  array&
  operator=(array other) // copy-and-swap
  {
    swap(*this, other);
    return *this;
  }

  value_type  operator[](std::size_t n) const noexcept {return impl[n];}
  value_type& operator[](std::size_t n)       noexcept {return impl[n];}

  template <typename Exp>
  auto
  operator+(Exp&& rhs)
  const noexcept
  -> array_exp<std::plus<>, const array&, decltype(rhs)>
  {
    return {*this, std::forward<Exp>(rhs)};
  }

  template <typename Exp>
  auto
  operator*(Exp&& rhs)
  const noexcept
  -> array_exp<std::multiplies<>, const array&, decltype(rhs)>
  {
    return {*this, std::forward<Exp>(rhs)};
  }

  template <typename Exp>
  explicit
  array(Exp&& rhs)
    : impl{}
  {
    for (auto i = 0u; i < N; ++i)
    {
      impl[i] = rhs[i];
    }
  }

  template <typename Op, typename Lhs, typename Rhs>
  array&
  operator=(array_exp<Op, Lhs, Rhs>&& rhs)
  noexcept
  {
    for (auto i = 0u; i < N; ++i)
    {
      impl[i] = rhs[i];
    }
    return *this;
  }

  // template <typename Exp>
  // array&
  // operator+=(Exp&& rhs) // accept both array_exp and array
  // noexcept
  // {
  //   for (auto i = 0u; i < N; ++i)
  //   {
  //     impl[i] = impl[i] + rhs[i];
  //   }
  //   return *this;
  // }

  friend
  std::ostream&
  operator<<(std::ostream& os, const array& a)
  {
    os << '{';
    if (dimension > 0)
    {
      std::copy(begin(a.impl), std::prev(end(a.impl)), std::ostream_iterator<value_type>(os, ","));
      os << *std::prev(end(a.impl));
    }
    return os << '}';
  }

  friend
  void
  swap(array& lhs, array& rhs)
  noexcept
  {
    std::swap(lhs.impl, rhs.impl);
  }
};

/* ---------------------------------------------------------------------------------------------- */

int main()
{
  auto e = (array<3>{1,2,3}+ array<3>{1,1,1}) * (array<3>{1,2,3} + array<3>{1,1,1});
  std::cout << static_cast<array<3>>(e) << '\n';
  // void* p = e;

  return 0;
}
