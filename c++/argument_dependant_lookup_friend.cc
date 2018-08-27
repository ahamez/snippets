// http://stackoverflow.com/a/382077/21584
#include <iostream>

class bar
{};

class foo
{
public:

  foo(const bar&) : m_value{42} {}

  foo(int x) : m_value{x} {}

  friend bool operator==(const foo& lhs, const foo& rhs){return lhs.m_value == rhs.m_value;}

private:

  friend bool operator<(const foo&, const foo&);
  int m_value;
};


bool operator<(const foo& lhs, const foo& rhs){return lhs.m_value < rhs.m_value;}

int main (int argc, char const *argv[])
{
  foo f0{0};
  foo f1{1};
  bar b0;
  bar b1;

  std::cout << std::boolalpha;
  std::cout << (f0 == f1) << '\n';
  std::cout << (f0 < f1) << '\n';

  std::cout << (f1 < b0) << '\n';
  std::cout << (b1 < b0) << '\n';
  std::cout << (f1 == b0) << '\n';
  // std::cout << (b1 == b0) << '\n'; // Cannot compile ADL won't work

  return 0;
}
