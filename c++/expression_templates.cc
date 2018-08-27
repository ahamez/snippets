#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>
#include <initializer_list>
#include <vector>

/* ---------------------------------------------------------------------------------------------- */

template <typename T, typename Exp>
struct vector_exp
{
  using container_type = std::vector<T>;
  using value_type = typename container_type::value_type;

  std::size_t size() const noexcept{return get().size();}

        Exp& get()       noexcept {return static_cast<      Exp&>(*this);}
  const Exp& get() const noexcept {return static_cast<const Exp&>(*this);}
};

/* ---------------------------------------------------------------------------------------------- */

template <typename T>
struct vector
  : public vector_exp<T, vector<T>>
{
  using container_type = typename vector_exp<T, vector<T>>::container_type;
  using value_type = typename container_type::value_type;

  container_type impl;

  vector(std::initializer_list<int> il)
  noexcept
    : impl{begin(il), end(il)}
  {}

  vector() = delete;
  vector(const vector&) = default;
  vector(vector&&) = default;

  vector&
  operator=(vector other)
  {
    swap(*this, other);
    return *this;
  }

  value_type  operator[](std::size_t i) const noexcept {return impl[i];}
  value_type& operator[](std::size_t i)       noexcept {return impl[i];}

  std::size_t
  size()
  const noexcept
  {
    return impl.size();
  }

  template <typename E>
  vector(const vector_exp<T, E>& vector_exp)
  {
    impl.resize(vector_exp.get().size());
    for (auto i = 0ul; i < vector_exp.get().size(); ++i)
    {
      impl[i] = vector_exp.get()[i];
    }
  }

  friend
  std::ostream&
  operator<<(std::ostream& os, const vector& a)
  {
    os << '{';
    if (a.impl.size() > 0)
    {
      std::copy(begin(a.impl), std::prev(end(a.impl)), std::ostream_iterator<value_type>(os, ","));
      os << *std::prev(end(a.impl));
    }
    return os << '}';
  }

  friend
  void
  swap(vector& lhs, vector& rhs)
  noexcept
  {
    std::swap(lhs.impl, rhs.impl);
  }
};

/* ---------------------------------------------------------------------------------------------- */

template <typename Op, typename T, typename E>
struct apply_value
  : public vector_exp<T, apply_value<Op, T, E>>
{
  using value_type = typename vector_exp<T, apply_value<Op, T, E>>::value_type;

  T value;
  E sub;

  apply_value() = delete;
  apply_value(const apply_value&) = default;

  apply_value(apply_value&&) = default;
  apply_value& operator=(apply_value&&) = default;

  apply_value(T v, vector_exp<T, E>&& e)
    : value(v), sub(std::move(e.get()))
  {}

  apply_value(T v, const vector_exp<T, E>& e)
    : value(v), sub(e.get())
  {}

  std::size_t size()                    const noexcept {return sub.size();}
  value_type  operator[](std::size_t i) const noexcept {return Op{}(value, sub[i]);}
};

#define CREATE_APPLY_OP(NAME,OP,FN)                    \
                                                       \
template <typename T, typename E>                      \
using NAME = apply_value<FN, T, E>;                    \
                                                       \
template <typename T, typename E>                      \
inline                                                 \
NAME<T, E>                                             \
operator OP (T value, vector_exp<T, E>&& e)            \
{                                                      \
  return {value, std::move(e)};                        \
}                                                      \
                                                       \
template <typename T, typename E>                      \
inline                                                 \
NAME<T, E>                                             \
operator OP (vector_exp<T, E>&& e, T value)            \
{                                                      \
  return {value, std::move(e)};                        \
}                                                      \
                                                       \
template <typename T, typename E>                      \
inline                                                 \
NAME<T, E>                                             \
operator OP (T value, const vector_exp<T, E>& e)       \
{                                                      \
  return {value, e};                                   \
}                                                      \
                                                       \
template <typename T, typename E>                      \
inline                                                 \
NAME<T, E>                                             \
operator OP (const vector_exp<T, E>& e, T value)       \
{                                                      \
  return {value, e};                                   \
}

CREATE_APPLY_OP( value_plus_exp        , + , std::plus<>       );
CREATE_APPLY_OP( value_minus_exp       , - , std::minus<>      );
CREATE_APPLY_OP( value_multiplies_exp  , * , std::multiplies<> );
CREATE_APPLY_OP( value_divide_exp      , / , std::divides<>    );

/* ---------------------------------------------------------------------------------------------- */

template <typename Op, typename T, typename E1, typename E2>
struct binary
  : public vector_exp<T, binary<Op, T, E1, E2>>
{
  using value_type = typename vector_exp<T, binary<Op, T, E1, E2>>::value_type;

  E1 lhs;
  E2 rhs;

  binary(vector_exp<T, E1>&& l, vector_exp<T, E2>&& r)
    : lhs(std::move(l.get())), rhs(std::move(r.get()))
  {}

  binary(const vector_exp<T, E1>& l, vector_exp<T, E2>&& r)
    : lhs(l.get()), rhs(std::move(r.get()))
  {}

  binary(vector_exp<T, E1>&& l, const vector_exp<T, E2>& r)
    : lhs(std::move(l.get())), rhs(r.get())
  {}

  binary(const vector_exp<T, E1>& l, const vector_exp<T, E2>& r)
    : lhs(l.get()), rhs(r.get())
  {}

  std::size_t
  size()
  const noexcept
  {
    return lhs.size();
  }

  value_type
  operator[](std::size_t i)
  const noexcept
  {
    return Op{}(lhs[i], rhs[i]);
  }
};

#define CREATE_BINARY_OP(NAME,OP,FN)                                     \
                                                                         \
template <typename T, typename E1, typename E2>                          \
using NAME = binary<FN, T, E1, E2>;                                      \
                                                                         \
template <typename T, typename E1, typename E2>                          \
inline                                                                   \
NAME<T, E1, E2>                                                          \
operator OP (vector_exp<T, E1>&& lhs, vector_exp<T, E2>&& rhs)           \
{                                                                        \
  return {std::move(lhs), std::move(rhs)};                               \
}                                                                        \
                                                                         \
template <typename T, typename E1, typename E2>                          \
inline                                                                   \
NAME<T, E1, E2>                                                          \
operator OP (const vector_exp<T, E1>& lhs, vector_exp<T, E2>&& rhs)      \
{                                                                        \
  return {lhs, std::move(rhs)};                                          \
}                                                                        \
                                                                         \
template <typename T, typename E1, typename E2>                          \
inline                                                                   \
NAME<T, E1, E2>                                                          \
operator OP (vector_exp<T, E1>&& lhs, const vector_exp<T, E2>& rhs)      \
{                                                                        \
  return {std::move(lhs), rhs};                                          \
}                                                                        \
                                                                         \
template <typename T, typename E1, typename E2>                          \
inline                                                                   \
NAME<T, E1, E2>                                                          \
operator OP (const vector_exp<T, E1>& lhs, const vector_exp<T, E2>& rhs) \
{                                                                        \
  return {lhs, rhs};                                                     \
}

CREATE_BINARY_OP( plus_exp        , + , std::plus<>       );
CREATE_BINARY_OP( minus_exp       , - , std::minus<>      );
CREATE_BINARY_OP( multiplies_exp  , * , std::multiplies<> );
CREATE_BINARY_OP( divides_exp     , / , std::divides<>    );

/* ---------------------------------------------------------------------------------------------- */

int main()
{
  using vec = vector<int>;
  auto dummy = 0ul;

  // // for (auto i = 0ul; i < 9999999; ++i)
  // {
  //   const auto v = (vec{0,1,2,3,4,5,6,7,8,9} * 3)
  //                + (vec{1,1,2,3,1,5,1,7,1,9} + vec{1,1,2,3,1,5,1,7,1,9})
  //                - (vec{9,9,9,9,9,9,9,9,9,9} / vec{2,2,2,2,2,2,2,2,2,2})
  //               ;
  //   // for (auto j = 0ul; j < 10; ++j)
  //   {
  //     dummy += v[0];
  //   }
  // }
  //
  // std::cout << dummy << '\n';
  std::cout << (vec{0,1,2} + 3) << ('\n');

  return 0;
}
