
#include <iostream>


template <std::uint8_t Opcode>
struct instruction;

template <>
struct instruction<0>
{
  void operator()(){std::cout << 0 << '\n';}
};

template <>
struct instruction<1>
{
  void operator()(){std::cout << 1 << '\n';}
};

template <>
struct instruction<2>
{
  void operator()(){std::cout << 2 << '\n';}
};

template <std::uint8_t Opcode>
void
execute()
{
  instruction<Opcode>{}();
}

template <std::uint8_t... Opcodes>
void
step(std::uint8_t opcode, std::integer_sequence<std::uint8_t, Opcodes...>)
{
  if (opcode > 2)
  {
    throw std::runtime_error{"Invalid opcode"};
  }

  using fun_ptr_type = void (*) ();
  static constexpr fun_ptr_type jump_table[] = {&execute<Opcodes>...};
  jump_table[opcode]();
}

int main()
{
  step(0, std::make_integer_sequence<std::uint8_t, 3>{});
  step(1, std::make_integer_sequence<std::uint8_t, 3>{});
  step(2, std::make_integer_sequence<std::uint8_t, 3>{});
}