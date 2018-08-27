#include <iostream>

////////////////////////////////////////////////////////////////////////////////

template <typename... Types>
struct foo
{
};

////////////////////////////////////////////////////////////////////////////////

template < typename Y
         , typename... Types1, template <typename...> class T
         , typename W
         , typename... Types2, template <typename...> class U
         , typename Z
         >
void
bar(const Y& y, const T<Types1...>&, const U<Types2...>&, const Z& z, const W& w)
{
  std::cout << y << std::endl;
  std::cout << sizeof...(Types1) << std::endl;
  std::cout << w << std::endl;
  std::cout << sizeof...(Types2) << std::endl;
  std::cout << z << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

template <size_t Value>
struct baz
{
  template < typename... Types1, template <typename...> class T
           , typename... Types2, template <typename...> class U
           >
  void
  operator()(const T<Types1...>&, const U<Types2...>&)
  const
  {
    std::cout << "baz::operator()" << std::endl;
    std::cout << sizeof...(Types1) << std::endl;
    std::cout << sizeof...(Types2) << std::endl;
    std::cout << Value << std::endl;
  }
};


////////////////////////////////////////////////////////////////////////////////

int
main()
{
  foo<char, int, float> f;
  foo<char, int> g;
  bar(2, f, g, 9, 42);
  baz<0>()(f, g);
  return 0;
}