#include <functional>
#include <iostream>
#include <string>
#include <variant>

template <typename... Ts>
struct make_visitor
  : Ts...
{
  using Ts::operator()...;
};

// Deduction guide.
// - https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
// - https://stackoverflow.com/q/40951697/21584
template <typename... Ts>
make_visitor(Ts...) -> make_visitor<Ts...>;

using variant = std::variant<int, std::string, double>;

int main()
{
  {
    const auto v0 = variant{42.2};
  
    std::visit(make_visitor{
      [](int){std::cout << "int\n";},
      [](std::string){std::cout << "std::string\n";},
      [](double){std::cout << "double\n";}
    }, v0);
  
    // Without deduction guide
    std::visit( make_visitor<
        std::function<void(int)>, 
        std::function<void(std::string)>,
        std::function<void(double)>>
    {
      [](int){std::cout << "int\n";},
      [](std::string){std::cout << "std::string\n";},
      [](double){std::cout << "double\n";}
    }, v0);
  }
}
