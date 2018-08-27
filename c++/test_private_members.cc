#include <iostream>

template <typename T> struct tester;

class foo
{
  int x_;

  template <typename T> friend struct tester;
};

template <>
struct tester<foo>
{
  static int x(const foo& f)
  {
    return f.x_;
  }
};

int main (int argc, char const *argv[])
{
  auto f = foo{};
  std::cout << tester<foo>::x(f) << '\n';
  return 0;
}