#include <numeric>
#include <iostream>
#include <vector>

template <typename T>
auto foo = T(42);

template <>
auto foo<double> = 33.33;

template <std::size_t X>
auto bar = X;

template <>
auto bar<42> = "Don't panic";

// -------------------------------------

template <typename...>
struct variadic_counter;

template <typename T, typename... Ts>
struct variadic_counter<T, Ts...>
{
  static constexpr auto value = 1ul + variadic_counter<Ts...>::value;
};

template <>
struct variadic_counter<>
{
  static constexpr auto value = 0;
};

template <typename... Ts>
auto v = variadic_counter<Ts...>::value;

// -------------------------------------

template <unsigned...>
struct nth_impl;

template <unsigned X, unsigned... Xs>
struct nth_impl<0, X, Xs...>
{
  static constexpr auto value = X;
};

template <unsigned Index, unsigned X, unsigned... Xs>
struct nth_impl<Index, X, Xs...>
{
  static constexpr auto value = nth_impl<Index - 1, Xs...>::value;
};

template <unsigned Index, unsigned... Xs>
auto nth = nth_impl<Index, Xs...>::value;

// -------------------------------------

template <unsigned Index, unsigned X, unsigned... Xs>
auto nth2 = nth2<Index - 1, Xs...>;

template <unsigned X, unsigned... Xs>
auto nth2<0, X, Xs...> = X;

// -------------------------------------

int main (int argc, char const *argv[])
{
  std::cout << foo<double> << '\n';
  std::cout << bar<33> << '\n';
  std::cout << bar<42> << '\n';

  std::cout << v<int,char,double,char,int> << '\n';
  std::cout << nth<1,  1,2,3,4> << '\n';
  std::cout << nth2<0,  1,2,3,4> << '\n';

  return 0;
}