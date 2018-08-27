#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "encode.hh"
#include "predefined.hh"

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char** argv)
{
  using namespace codes;
  namespace chrono = std::chrono;

  const auto file_name = [&]
  {
      if (argc == 2u) return std::string{argv[1]};
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

  const auto file_size = [&]
  {
    const auto res = std::distance( std::istreambuf_iterator<char>{file}
                                  , std::istreambuf_iterator<char>{});
    file.clear();
    file.seekg(0);
    return res;
  }();

  if ((file_size % 4096) != 0)
  {
    std::cerr << "file size if not a multiple of 4096\n";
    return 1;
  }

  std::ofstream r1_file{file_name + ".r1"};
  std::ofstream r2_file{file_name + ".r2"};  

  auto r1_stream = std::ostreambuf_iterator<char>{r1_file};
  auto r2_stream = std::ostreambuf_iterator<char>{r2_file};

  const auto file_mem = [&]
  {
    auto res = std::vector<char>{};
    res.reserve(file_size);
    std::copy( std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}
             , std::back_inserter(res));
    return res;
  }();

  auto r1 = vector_type{};
  auto r2 = vector_type{};

  auto k1 = vector_type{};
  auto k2 = vector_type{};
  auto k3 = vector_type{};
  auto k4 = vector_type{};

  // Convenient holder for all k inputs.
  const auto input = input_type<4>{begin(k1), begin(k2), begin(k3), begin(k4)};
 
  // for (auto loop = 0ul; loop < 100; ++loop)
  for (auto counter = 0ul; counter < (file_mem.size()/vector_size); counter += 4)
  {
    std::copy_n( begin(file_mem) + (counter * vector_size + vector_size * 0)
               , vector_size
               , begin(k1));
    std::copy_n( begin(file_mem) + (counter * vector_size + vector_size * 1)
               , vector_size
               , begin(k2));
    std::copy_n( begin(file_mem) + (counter * vector_size + vector_size * 2)
               , vector_size
               , begin(k3));
    std::copy_n( begin(file_mem) + (counter * vector_size + vector_size * 3)
               , vector_size
               , begin(k4));

    start = chrono::system_clock::now();
    encode<xor_>    (r1, input);
    encode<c1_2_4_9>(r2, input);
    duration += (chrono::system_clock::now() - start);

    std::copy(begin(r1), end(r1), r1_stream);
    std::copy(begin(r2), end(r2), r2_stream);
  }
 
  std::cout << duration.count() << '\n';
  std::cout << ((file_mem.size()/1024/1024)/duration.count()) << " Mbytes/s\n";
 
  //
  // std::copy(begin(r1), end(r1), std::ostreambuf_iterator<char>{r1_file});
  // std::copy(begin(r2), end(r2), std::ostreambuf_iterator<char>{r2_file});

  return 0;
}
