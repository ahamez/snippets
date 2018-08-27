#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include "encode.hh"

/*------------------------------------------------------------------------------------------------*/

using namespace codes;

/*------------------------------------------------------------------------------------------------*/

template <typename, typename...>
struct cons_krow;

template <typename K, typename... Ks>
struct cons_krow<K, krow<Ks...>>
{
  using type = krow<K, Ks...>;
};

/*------------------------------------------------------------------------------------------------*/

template <typename...>
struct cat_krow;

template <typename... Ls, typename... Rs>
struct cat_krow<krow<Ls...>, krow<Rs...>>
{
  using type = krow<Ls..., Rs...>;
};

/*------------------------------------------------------------------------------------------------*/

template <std::size_t KIndex, std::size_t, bool...>
struct mk_krow_impl;

template <std::size_t KIndex, std::size_t Index, bool Bvalue>
struct mk_krow_impl<KIndex, Index, Bvalue>
{
  using type = conditional_t< Bvalue 
                            , krow<k<KIndex, Index>>
                            , krow<>
                            >;
};

template <std::size_t KIndex, std::size_t Index, bool Bvalue, bool... Bvalues>
struct mk_krow_impl<KIndex, Index, Bvalue, Bvalues...>
{
  using rec = typename mk_krow_impl<KIndex, Index + 1, Bvalues...>::type;
  using type = conditional_t< Bvalue 
                            , typename cons_krow<k<KIndex, Index>, rec>::type
                            , rec
                            >;
};


template <std::size_t, typename...>
struct mk_krow;

template <std::size_t KIndex, bool... Bvalues>
struct mk_krow<KIndex, brow<Bvalues...>>
{
  using type = typename mk_krow_impl<KIndex, 0, Bvalues...>::type;
};

/*------------------------------------------------------------------------------------------------*/

// template <std::size_t, typename...>
// struct mk_code_col_iteration;
//
// template <std::size_t Row, typename... Brows, typename... Cs>
// struct mk_code_col_iteration<Row, coefficient<Brows...>, Cs...>
// {
//   using rec = typename mk_code_col_iteration<Row - 1, Cs...>::type;
//   using current = typename mk_krow<Row, Brows...>::type;
//   using type = typename cat_krow<current, rec>::type;
// };
//
// template <typename... Brows>
// struct mk_code_col_iteration<0, coefficient<Brows...>>
// {
//   using type = typename mk_krow<0, Brows...>::type;
// };

/*------------------------------------------------------------------------------------------------*/

template <typename... Cs>
struct mk_code
{
  template <typename C>
  struct same_size_impl
  {
    using hd = typename head<Cs...>::type;
    static constexpr auto value = C::size == hd::size;
  };
  static constexpr auto same_size = for_all<same_size_impl, Cs...>::value;
  static_assert(same_size, "All coefficients must have the same size");

  using type = bool;
};

/*------------------------------------------------------------------------------------------------*/

template <typename T> void type() {std::cout << __PRETTY_FUNCTION__ << '\n';}

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  using c1 = coefficient< brow<0,0,0>
                        , brow<0,1, 0>
                        , brow<0,1, 1>
                        >;

  using c2 = coefficient< brow<1,0>
                        , brow<1,0>
                        >;

  using r0k0 = mk_krow<0, brow<0,1,1,0,1>>::type;
  using r0k1 = mk_krow<1, brow<0,1,1,0,0>>::type;
  type<r0k0>();
  type<r0k1>();

  using r0 = cat_krow<r0k0, r0k1>::type;
  type<r0>();

  // using r0 = mk_code_col_iteration<1, c1, c2>::type;

  // print_type<mk_code<c1,c2>::type>();

  return 0;
}
