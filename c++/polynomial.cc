#include <array>
#include <cassert>
#include <cmath>        // pow
#include <functional>   // invoke
#include <iostream>     // ostream
#include <type_traits>  // is_same
#include <utility>      // integer_sequence

namespace newton::meta
{

/* ------------------------------------------------------------------------------------------------------------------ */

// Check whether passed types have the same type.
template <typename T1, typename... TN>
struct HomogeneousTypes
{
  static constexpr bool value = (std::is_same_v<T1, TN> && ...);
};

// Check whether passed arguments have the same type.
template <typename T1, typename... TN>
constexpr bool is_homogeneous(T1, TN...)
{
  return (std::is_same_v<T1, TN> && ...);
}

/* ------------------------------------------------------------------------------------------------------------------ */

template <typename T, typename...>
struct Head
{
  using type = T;
};

template <typename... Ts>
using HeadType = typename Head<Ts...>::type;

/* ------------------------------------------------------------------------------------------------------------------ */

template <std::size_t Shift, typename T, T... Is>
constexpr auto shift_integer_sequence(std::integer_sequence<T, Is...>)
{
  return std::integer_sequence<T, (Is + Shift)...>{};
}

template <typename T, std::size_t N, std::size_t Start = 0>
constexpr auto make_integer_sequence()
{
  return shift_integer_sequence<Start>(std::make_integer_sequence<T, N>{});
}

/* ------------------------------------------------------------------------------------------------------------------ */

}  // namespace newton::meta

namespace newton::maths
{

template <typename... Args>
auto make_polynomial(Args&&... args) noexcept;

namespace detail
{

template <typename T, unsigned Exponent>
struct Coefficient
{
  explicit Coefficient(T coefficient)
      : coefficient{coefficient}
  {
  }

  T coefficient;
  [[nodiscard]] auto apply(T x) const noexcept { return coefficient * std::pow(x, Exponent); }

 protected:
  [[nodiscard]] T get_coefficient() const noexcept { return coefficient; }
};

template <typename T, unsigned... exponents>
struct Polynomial final : private Coefficient<T, exponents>...
{
  static constexpr auto degree_v = sizeof...(exponents) - 1;

  template <typename... C>
  constexpr explicit Polynomial(C... c) noexcept
      : Coefficient<T, exponents>{c}...
  {
    static_assert(sizeof...(C) == sizeof...(exponents), "Invalid number of coefficients for the given degree.");
  }

  [[nodiscard]] constexpr auto operator()(T x) const noexcept { return (... + Coefficient<T, exponents>::apply(x)); }

  [[nodiscard]] constexpr auto degree() const noexcept { return degree_v; }

  template <unsigned Exponent>
  [[nodiscard]] T coefficient() const noexcept
  {
    static_assert(Exponent < sizeof...(exponents));
    return Coefficient<T, Exponent>::coefficient;
  }

  [[nodiscard]] T coefficient(unsigned exponent) const noexcept
  {
    using fun_ptr_type = T (Polynomial::*)() const;
    constexpr auto table = std::array<fun_ptr_type, sizeof...(exponents)>{&Polynomial::coefficient<exponents>...};
    assert(exponent < sizeof...(exponents));
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return std::invoke(table[exponent], this);
  }

  friend std::ostream& operator<<(std::ostream& os, const Polynomial& polynomial)
  {
    os << polynomial.coefficient<0>();
    if constexpr (Polynomial::degree_v >= 1)
    {
      os << " + " << polynomial.coefficient<1>() << "x";
      for (auto exp = unsigned{2}; exp <= polynomial.degree(); ++exp)
      {
        os << " + " << polynomial.coefficient(exp) << "x^" << exp;
      }
    }
    return os;
  }

  [[nodiscard]] auto derive() const noexcept
  {
    return derive_impl(meta::make_integer_sequence<unsigned, degree_v, /* start = */ 1>());
  }

 private:
  template <unsigned... exps>
  [[nodiscard]] auto derive_impl(std::integer_sequence<unsigned, exps...>) const noexcept
  {
    return make_polynomial(exps * coefficient<exps>()...);
  }
};

template <typename T, typename Seq>
struct MakePolynomialHelper;

template <typename T, typename U, U... Indexes>
struct MakePolynomialHelper<T, std::integer_sequence<U, Indexes...>> final
{
  using type = detail::Polynomial<T, Indexes...>;
};

}  // namespace detail

template <typename T, unsigned Degree>
using MakePolynomial =
    typename detail::MakePolynomialHelper<std::remove_pointer_t<std::remove_cv_t<std::remove_reference_t<T>>>,
                                          std::make_integer_sequence<unsigned, Degree + 1>>::type;

template <typename... Args>
auto make_polynomial(Args&&... args) noexcept
{
  static_assert(meta::HomogeneousTypes<Args...>::value, "Passed coefficients don't have the same type.");
  using polynomial_type = MakePolynomial<meta::HeadType<Args...>, sizeof...(Args) - 1>;
  return polynomial_type{std::forward<Args>(args)...};
}

}  // namespace newton::maths

int main()
{
  using namespace newton::maths;

  //  using Polynomial0 = MakePolynomial<double, 0>;
  //  using Polynomial1 = MakePolynomial<double, 1>;
  //  using Polynomial2 = MakePolynomial<double, 2>;
  //  using Polynomial3 = MakePolynomial<double, 3>;
  //  using Polynomial5 = MakePolynomial<double, 5>;

  //  constexpr auto c0 = Polynomial0{1.0}(3.3);
  //  std::cout << c0 << '\n';
  //  constexpr auto c1 = Polynomial1{1.0, -2.0}(3.3);
  //  std::cout << c1 << '\n';
  //  constexpr auto c2 = Polynomial2{1.0, -2.0, 3.0}(3.3);
  //  std::cout << c2 << '\n';
  //  constexpr auto c3 = Polynomial3{1.0, -2.0, 3.0, -4.0}(3.3);
  //  std::cout << c3 << '\n';
  //    const auto c5 = Polynomial5{1.0, -2.0, 3.0, -4.0, 5.0, -6.0}(3.3);
  //    std::cout << c5 << '\n';

  const auto p5 = make_polynomial(1.0, -2.0, 3.0, -4.0, 5.0, -6.0);
  //  std::cout << p5.coefficient<0>() << '\n';
  //  std::cout << p5.coefficient<1>() << '\n';
  //  std::cout << p5.coefficient<2>() << '\n';
  //  std::cout << p5.coefficient<3>() << '\n';
  //  std::cout << p5.coefficient<4>() << '\n';
  std::cout << p5.coefficient<5>() << '\n';
  //
  //  std::cout << p5.coefficient(0) << '\n';
  //  std::cout << p5.coefficient(1) << '\n';
  //  std::cout << p5.coefficient(2) << '\n';
  //  std::cout << p5.coefficient(3) << '\n';
  //  std::cout << p5.coefficient(4) << '\n';
  //  std::cout << p5.coefficient(5) << '\n';

  const auto p3 = make_polynomial(1.0, -2.0, 3.0, -4.0);
  std::cout << p3 << '\n';
  // 0                  // 1                 // 2
  //  const auto p2 = make_polynomial(1 * p3.coefficient<1>(), 2 * p3.coefficient<2>(), 3 * p3.coefficient<3>());
  //  std::cout << p2 << '\n';

  //  std::cout << "A " << p3.derive_impl(std::integer_sequence<unsigned , 1, 2, 3>{}) << '\n';
  //  std::cout << "B " << p3.derive_impl(meta::shift_integer_sequence<unsigned, 3, 1>()) << '\n';
  std::cout << p3.derive() << '\n';

  std::cout << p5 << '\n';
  std::cout << p5.derive() << '\n';

  std::cout << make_polynomial(1.0) << '\n';
  std::cout << make_polynomial(1.0, 2.0) << '\n';

  //  std::string line;
  //
  //  std::getline(std::cin, line);
  //  const auto c0 = stod(line);
  //  std::getline(std::cin, line);
  //  const auto c1 = stod(line);
  //  std::getline(std::cin, line);
  //  const auto c2 = stod(line);
  //  std::getline(std::cin, line);
  //  const auto x = stod(line);
  //
  //  const auto p = make_polynomial(c0, c1, c2);
  //  std::cout << p(x) << '\n';
}
