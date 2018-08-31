// clang++ -Wall -Wextra -O3 -std=c++17 closure.cc

#include <iostream>

struct foo
{
  foo(){std::cout << this << " foo()\n";}
  foo(const foo& lhs){std::cout << &lhs << " foo(const foo&)\n";}
  foo(foo&& lhs){std::cout << &lhs << " foo(foo&&)\n";}
};

int main()
{
  auto f = foo{};

  [[maybe_unused]] const auto fn1 = [a = std::move(f)]{};
  [[maybe_unused]] const auto fn2 = [a = std::move(f)]{};
}