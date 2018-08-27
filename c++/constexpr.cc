#include <iostream>
#include <vector>

struct foo
{
  int data;
  
  constexpr
  foo(int d) : data(d) {}

  constexpr int operator()() const
    {return data + 1;}

  constexpr operator int() const
    {return data;}
};

constexpr foo bar() 
  {return foo{42};}

int main ()
{
            auto f0 = foo{42};
  constexpr auto f1 = foo{33};
  constexpr auto f2 = bar();

            auto i0 = f0();
  constexpr auto i1 = f1();
  constexpr auto i2 = static_cast<int>(f1);
            auto i3 = static_cast<int>(f1);
  return 0;
}