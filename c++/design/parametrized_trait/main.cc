#include <iostream>

#include "accum.hh"

template<>
class AccumulationTraits<long>
{
public:
  typedef long AccT;

  static
  AccT
  zero()
  {
    return 0;
  }
};

class OverridedAccumulationTraits
{
public:
  typedef int AccT;

  static
  AccT
  zero()
  {
    return 100;
  }
};

int main()
{
  int num[] = {1,2,3,4,5};
  char name[] = "toto";
  long tab[] ={1,2,10,128,254};

  // int type with default trait
  std::cout
    << Accum<int>::accum( &num[0] , &num[4] )
    << '\n';

  // int type with override
  std::cout
    << Accum< int , OverridedAccumulationTraits >::accum( &num[0] , &num[4] )
    << '\n';

  // char type
  std::cout
    << Accum<char>::accum( &name[0] , &name[5] )
    << '\n';

  // long type with custom trait
  std::cout
    << Accum<long>::accum( &tab[0] , &tab[5] )
    << '\n';

  return 0;
}
