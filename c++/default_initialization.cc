#include <array>
#include <iostream>

struct foo
{
  char i;

  // Uncomment the following to make sure foo is always initialized
  // foo()
  //   : i{}
  // {}
};

int
main ()
{
  auto a = std::array<char, 1024>{{'@', 'b', 'c', 'd', 'e', 'f', '\0'}};

  // f0.i is initialized
  const auto f0 = foo{};
  std::cout << "'" << +f0.i << "'\n";

  // f1.i isn't initialized, notice the missing '{}'
  const auto* f1 = new (a.data()) foo;
  std::cout << "'" << f1->i << "'\n";

  // f2.i is initialized
  const auto* f2 = new (a.data()) foo{};
  std::cout << "'" << +f2->i << "'\n";
}
