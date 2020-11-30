// clang++ -fsanitize=address -O0 -g -std=c++17 -lsodium sodium.cc

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <sodium.h>

// Keep this public key hardcoded.
static const auto public_key = std::vector<std::uint8_t>{
    178, 125, 179, 141, 119, 23, 200, 146, 102, 79, 175, 155, 64, 194, 150, 62,
    47, 100, 210, 113, 80, 4, 34, 241, 240, 115, 65, 230, 71, 173, 141, 16
  };

// Return an initialized optional with the verified data if the signature has been verified.
std::optional<std::vector<std::uint8_t>>
verify(const std::vector<std::uint8_t>& data, const std::vector<std::uint8_t>& public_key)
{
  if (data.size() < crypto_sign_BYTES)
  {
    // Data was smaller than the size of a signature. It cannot have been signed by our private key.
    return {};
  }

  auto verified_data = std::vector<std::uint8_t>(data.size() - crypto_sign_BYTES);

  const auto res = crypto_sign_open(
    verified_data.data(),
    nullptr,
    data.data(),
    data.size(),
    public_key.data()
  );

  if (res == 0)
  {
    return {std::move(verified_data)};
  }
  else
  {
    return {};
  }
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Missing path to file. Exiting.\n";
    return 1;
  }

  const auto file_path = argv[1];
  auto fstream = std::ifstream{file_path};
  const auto signed_data = std::vector<std::uint8_t>{
    std::istreambuf_iterator<char>(fstream),
    std::istreambuf_iterator<char>()
  };

  const auto res = verify(signed_data, public_key);
  if (res)
  {
    std::cout << "Data was signed by our private key :-)\n";
    // You can now work with res as you like!
    const auto str = std::string{res->cbegin(), res->cend()};
    std::cout << "String content is: \"" << str << "\"\n";
  }
  else
  {
    std::cout << "Data was not signed by our private key :-(";
  }

  return 0;
}
