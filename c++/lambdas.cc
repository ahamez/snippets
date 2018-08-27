#include <iostream>

auto list = [](auto... xs)
{
  return [=](auto access) { return access(xs...); };
};

auto cons = [](auto x)
{
  return [=](auto... xs){return list(x, xs...);};
};

auto len = [](auto... xs)
{
  return sizeof...(xs);
};

auto head = [](auto x, auto... xs)
{
  return x;
};

auto tail = [](auto x, auto... xs)
{
  return list(xs...);
};

auto map = [](auto fun)
{
  return [=](auto... xs){return list(fun(xs)...);};
};

template <typename Fun, typename T>
auto apply_impl(const Fun& fun, const T& x)
{
  fun(x);
}

template <typename Fun, typename T, typename... Ts>
auto apply_impl(const Fun& fun, const T& x, Ts&&... ts)
{
  fun(x);
  apply_impl(fun, std::forward<Ts>(ts)...);
}

auto apply = [=](auto fun)
{
  return [=](auto... xs){apply_impl(fun, xs...); return list(xs...);};
};

// Can't compile because list(x, xs...) and list(xs...) have different types
// auto filter = [](auto pred)
// {
//   return [=](auto x, auto... xs){return pred(x) ? list(x, xs...): list(xs...);};
// };

int main()
{
  std::cout << list(1,2,3)(len) << std::endl;
  std::cout << list()(len) << std::endl;

  std::cout << (list())(cons(2))(cons(1))(len) << std::endl;

  std::cout << "Map" << std::endl;
  auto f = [](auto x){return x * 2;};
  std::cout << list(1,2,3)(map(f))(head) << std::endl;
  std::cout << list(1,2,3)(map(f))(tail)(head) << std::endl;
  std::cout << list(1,2,3)(map(f))(tail)(tail)(head) << std::endl;
  std::cout << std::endl;

  std::cout << "Apply" << std::endl;
  auto print = [](auto x){std::cout << x << ", ";};
  auto print_end = [](auto x){std::cout << x;};
  list(1,2,3)(apply(print));
  std::cout << std::endl;

  return 0;
}
