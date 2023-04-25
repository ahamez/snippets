#include <iostream>
#include <utility>

namespace
{

template <typename T>
void fun(T x)
{
  std::cout << "fun(" << x << ")\n";
}

template <typename... T>
void call(T... args)
{
  (..., fun(args));
}

template <typename T, T... ints>
void call(std::integer_sequence<T, ints...>)
{
  call(ints...);
}

}  // namespace

int main()
{
  call(std::make_index_sequence<20>{});

  return 0;
}
