#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>

#include "encode.hh"
#include "predefined.hh"

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  using namespace codes;
  namespace chrono = std::chrono;

  const auto nb_loops = argc == 1u ? 10u : std::stoi(argv[1]);

  static constexpr auto dummy_bytes     = 1024 * 1024 * 1024;
  static constexpr auto dummy_data_size = dummy_bytes / sizeof(vector_data_type);

  // Allocate some memory and don't initialize it to have some dummy data.
  const auto dummy_data = std::make_unique<std::array<vector_data_type, dummy_data_size>>();

  auto r1 = vector_type{};
  auto r2 = vector_type{};

   auto k1 = vector_type{};
   auto k2 = vector_type{};
   auto k3 = vector_type{};
   auto k4 = vector_type{};

  // Convenient holder for all k inputs.
  const auto input = input_type<4>{begin(k1), begin(k2), begin(k3), begin(k4)};

  auto start    = chrono::time_point<chrono::system_clock>{};
  auto duration = chrono::duration<double>::zero();
  const auto mbytes = static_cast<unsigned long long>(dummy_bytes) * nb_loops/1024/1024;

  for (auto i = 0ul; i < nb_loops; ++i)
  for (auto counter = 0ul; counter < (dummy_data_size/vector_size); counter += 4)
  {
    std::copy_n( begin(*dummy_data) + (counter * vector_size + vector_size * 0)
               , vector_size
               , begin(k1));
    std::copy_n( begin(*dummy_data) + (counter * vector_size + vector_size * 1)
               , vector_size
               , begin(k2));
    std::copy_n( begin(*dummy_data) + (counter * vector_size + vector_size * 2)
               , vector_size
               , begin(k3));
    std::copy_n( begin(*dummy_data) + (counter * vector_size + vector_size * 3)
               , vector_size
               , begin(k4));

    start = chrono::system_clock::now();
    encode<xor_>    (r1, input);
    encode<c1_2_4_9>(r2, input);
    duration += (chrono::system_clock::now() - start);
  }

  std::cout << "R1 R2 Duration  : " << duration.count() << "s\n";
  std::cout << "R1 R2 Rate      : " << mbytes/duration.count()
            << " Mbytes/s\n";
 
  // Test and bench against simple XOR
  auto oracle = vector_type{};

  std::fill(begin(r1), end(r1), -42);
  std::fill(begin(oracle), end(oracle), 42);

  std::fill(begin(k1), end(k1), 1);
  std::fill(begin(k2), end(k2), 2);
  std::fill(begin(k3), end(k3), 4);
  std::fill(begin(k4), end(k4), 8);

  duration = chrono::duration<double>::zero();
  for (auto i = 0ul; i < nb_loops; ++i)
  for (auto counter = 0ul; counter < (dummy_data_size/vector_size); counter += 4)
  {
    start = chrono::system_clock::now();
    for (auto j = 0ul; j < vector_size; ++j)
    {
      oracle[j] = k1[j] xor k2[j] xor k3[j] xor k4[j];
    }
    duration += (chrono::system_clock::now() - start);
  }

  std::cout << "XOR 1 Duration  : " << duration.count() << "s\n";
  std::cout << "XOR 1 Rate      : " << mbytes/duration.count()
            << " Mbytes/s\n";

  duration = chrono::duration<double>::zero();
  for (auto i = 0ul; i < nb_loops; ++i)
  for (auto counter = 0ul; counter < (dummy_data_size/vector_size); counter += 4)
  {
    start = chrono::system_clock::now();
    encode<xor_>(r1, input);
    duration += (chrono::system_clock::now() - start);
  }
  std::cout << "XOR 2 Duration  : " << duration.count() << "s\n";
  std::cout << "XOR 2 Rate      : " << mbytes/duration.count()
            << " Mbytes/s\n";

  // Test XOR
  std::cout << "Valid XOR2      : "
            << std::boolalpha << std::equal(begin(r1), end(r1), begin(oracle)) << '\n';

  // Use R2 to make sure the compiler doesn't get rid of it.
  return std::all_of(begin(r2), end(r2), [](auto x){return x == 42;});
}
