#include <iostream>

struct foo
{
  void
  operator()()
  && // <= 'ref-qualifier'
  {
    std::cout << "rvalue" << std::endl;
  }

  void
  operator()()
  & // <= 'ref-qualifier'
  {
    std::cout << "lvalue" << std::endl;
  }
};

int
main()
{
  foo f;
  f{}; // "lvalue"
  foo{}(); // "rvalue"

  return 0;
}