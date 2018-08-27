#include <iostream>
#include <vector>

template <typename... Ts>
struct tuple {};

template <typename T, typename... Ts>
struct tuple<T, Ts...>
  : tuple<Ts...>
{
  tuple(T t, Ts... ts)
    : tuple<Ts...>(ts...)
    , current(t)
  {}

  T current;

};

template <std::size_t, typename... Ts>
struct get_impl;

template <typename T, typename... Ts>
struct get_impl<0, tuple<T, Ts...>>
{
  using type = T;
  
  static
  type&
  get(tuple<T, Ts...>& t)
  {
    return t.current;
  }
};

template <std::size_t Index, typename T, typename... Ts>
struct get_impl<Index, tuple<T, Ts...>>
{
  using type = typename get_impl<Index - 1, tuple<Ts...>>::type;

  static
  type&
  get(tuple<T, Ts...>& t)
  {
    tuple<Ts...>& base = t;
    return get_impl<Index - 1, tuple<Ts...>>::get(t);
  }
};

template <std::size_t Index, typename... Ts>
auto
get(tuple<Ts...>& t)
-> decltype(get_impl<Index, tuple<Ts...>>::get(t))
{
  return get_impl<Index, tuple<Ts...>>::get(t);
}

template <typename T> void ty(){std::cout << __PRETTY_FUNCTION__ << '\n';}

int main()
{
  using tuple_ty = tuple<char, double, int>;
  auto t = tuple<char, double, int>{'a', 2.2, 3};
  // ty<get_impl<1, tuple_ty>::type>();
  auto x = get_impl<2, tuple_ty>::get(t);
  // std::cout << x << '\n';
  std::cout << get<0>(t);
}