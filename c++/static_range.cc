#include <array>
#include <iostream>
#include <utility>

template <std::size_t... Is>
constexpr
std::array<std::size_t, sizeof...(Is)>
make_range_impl(std::index_sequence<Is...>)
noexcept
{
  return {Is...};
}

template <std::size_t N>
constexpr
std::array<std::size_t, N>
make_range()
noexcept
{
  return make_range_impl(std::make_integer_sequence<std::size_t, N>{});
}

for (auto x : make_range<5>())
  std::cout << x << ",";