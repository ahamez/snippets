#include <iostream>

// The mixin
template <typename Derived>
struct comparisons
{
  friend
  bool
  operator==(const comparisons& lhs, const comparisons& rhs)
  noexcept
  {
      const Derived& d1 = derived(lhs);
      const Derived& d2 = derived(rhs);

      return not (d1 < d2) and not (d2 < d1);
  }

  friend
  bool
  operator!=(const comparisons& lhs, const comparisons& rhs)
  {
      return not (lhs == rhs);
  }

private:

  static
  const Derived&
  derived(const comparisons& c)
  noexcept
  {
    return static_cast<const Derived&>(c);
  }
};


struct foo
  : comparisons<foo> // Use CRTP
{
  friend
  bool
  operator<(const foo&, const foo& rhs)
  noexcept
  {
    return true;
  }
};

int main (int argc, char const *argv[])
{
  std::cout << (foo{} == foo{}) << '\n';
  return 0;
}
