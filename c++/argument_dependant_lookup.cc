#include <iostream>

namespace stl
{
  template <typename T>
  void baz(const T&){std::cout << "standard baz\n";}
}

namespace foo
{
  template <typename T>
  void algo(const T& x)
  {
    // If a baz function exists in the namespace of T, then it is called.
    using stl::baz; // Inject stl::baz into lookup
    baz(x);
  };
}

namespace fiz
{
  struct fuz
  {
    // Inlined friend free function: can only be called with a non-qualified call.
    friend void fun(const fuz&){};
  };

  // If uncommented, will be called
  // void baz(const fuz&){std::cout << "fiz::baz\n";}
}

int main (int argc, char const *argv[])
{
  foo::algo(fiz::fuz{});

  // Test inline friend free functions.
  fiz::fuz f;
  fun(f);
  // Fully qualified call doesn't compile
  // fiz::fun(f);

  return 0;
}
