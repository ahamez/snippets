#pragma once

#include <type_traits>

namespace codes {

/*------------------------------------------------------------------------------------------------*/

template <typename T>
constexpr
bool
is_power_of_two(T x)
{
  static_assert( std::is_integral<T>::value and std::is_unsigned<T>::value
               , "T should be an integral type and unsigned");
  return x and ((x & (x-1)) == 0);
}

/*------------------------------------------------------------------------------------------------*/

// True if Pred is true for all Xs, false otherwise.
template <template <typename> class Pred, typename... Xs>
struct for_all;

template <template <typename> class Pred, typename X>
struct for_all<Pred, X>
{
  static constexpr auto value = Pred<X>::value;
};

template <template <typename> class Pred, typename X, typename... Xs>
struct for_all<Pred, X, Xs...>
{
  static constexpr auto value = Pred<X>::value ? for_all<Pred, Xs...>::value : false;
};

/*------------------------------------------------------------------------------------------------*/

template <typename... Xs>
struct head;

template <typename X, typename... Xs>
struct head<X, Xs...>
{
  using type = X;
};

/*------------------------------------------------------------------------------------------------*/

// template <typename Head, template<typename...> class Tail>
// struct cons

/*------------------------------------------------------------------------------------------------*/

template<bool B, class T, class F>
using conditional_t = typename std::conditional<B,T,F>::type;

/*------------------------------------------------------------------------------------------------*/

} // namespace codes
