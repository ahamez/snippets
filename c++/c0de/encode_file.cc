#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <chrono>

#include "encode.hh"
#include "predefined.hh"

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  using namespace codes;
  namespace chrono = std::chrono;

  const auto file_name = [&]
  {
      if (argc == 2u) return argv[1];
      std::cerr << "No file\n";
      std::exit(1);
  }();

  auto start    = chrono::time_point<chrono::system_clock>{};
  auto duration = chrono::duration<double>::zero();

  std::ifstream file{file_name, std::ios::binary};
  if (not file.is_open())
  {
    std::cerr << "Can't read " << file_name << '\n';
    std::exit(1);
  }

  auto r1 = vector_type{};
  auto r2 = vector_type{};

  auto k1 = vector_type{};
  auto k2 = vector_type{};
  auto k3 = vector_type{};
  auto k4 = vector_type{};

  auto file_iterator = std::istreambuf_iterator<char>{file};
  std::copy_n(  file_iterator, vector_size, begin(k1));
  std::copy_n(++file_iterator, vector_size, begin(k2));
  std::copy_n(++file_iterator, vector_size, begin(k3));
  std::copy_n(++file_iterator, vector_size, begin(k4));

  // Convenient holder for all k inputs.
  const auto input = input_type<4>{begin(k1), begin(k2), begin(k3), begin(k4)};

  start = chrono::system_clock::now();
  encode<xor_>(r1, input);
  encode<c1_2_4_9>  (r2, input);
  duration = (chrono::system_clock::now() - start);

  std::cout << duration.count() << '\n';

  std::ofstream r1_file{"r1"};
  std::ofstream r2_file{"r2"};

  std::copy(begin(r1), end(r1), std::ostreambuf_iterator<char>{r1_file});
  std::copy(begin(r2), end(r2), std::ostreambuf_iterator<char>{r2_file});

  return 0;
}
