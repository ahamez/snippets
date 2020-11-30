#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include <sodium.h>

std::vector<std::uint8_t>
encrypt(const std::vector<std::uint8_t>& pk, const std::vector<std::uint8_t>& data)
{
  const auto size = crypto_box_SEALBYTES + data.size();
  auto encrypted = std::vector<std::uint8_t>(size);

  ::crypto_box_seal(encrypted.data(), data.data(), data.size(), pk.data());

  return encrypted;
}

int
main(int argc, char** argv)
{
  if (argc != 4)
  {
    std::cerr
      << "Usage: "
      << argv[0]
      << "/path/to/public_key /path/to/input_file /path/to/encrypted_output_file\n";

    return 1;
  }

  const auto public_key = [&]
  {
    const auto file_path = argv[1];
    auto fstream = std::ifstream{file_path, std::ios::binary};

    return std::vector<std::uint8_t>{
      std::istreambuf_iterator<char>(fstream),
      std::istreambuf_iterator<char>()};
  }();

  const auto file = [&]
  {
    const auto file_path = argv[2];
    auto fstream = std::ifstream{file_path, std::ios::binary};

    return std::vector<std::uint8_t>{
      std::istreambuf_iterator<char>(fstream),
      std::istreambuf_iterator<char>()};
  }();

  const auto encrypted = encrypt(public_key, file);

  const auto output_path = argv[3];
  auto fstream = std::ofstream{output_path, std::ios::binary};
  std::copy(cbegin(encrypted), cend(encrypted), std::ostream_iterator<char>{fstream});

  return 0;
}
