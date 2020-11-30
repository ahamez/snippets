// clang++ -fsanitize=address -O0 -g -std=c++17 -lsodium sodium.cc

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <sodium.h>

// #define MESSAGE (const unsigned char *) "Message"
// #define MESSAGE_LEN 7
// #define CIPHERTEXT_LEN (crypto_box_SEALBYTES + MESSAGE_LEN)

// /* Recipient creates a long-term key pair */

//  Anonymous sender encrypts a message using an ephemeral key pair
//  * and the recipient's public key
// unsigned char ciphertext[CIPHERTEXT_LEN];
// crypto_box_seal(ciphertext, MESSAGE, MESSAGE_LEN, recipient_pk);

// /* Recipient decrypts the ciphertext */
// unsigned char decrypted[MESSAGE_LEN];
// if (crypto_box_seal_open(decrypted, ciphertext, CIPHERTEXT_LEN,
//                          recipient_pk, recipient_sk) != 0) {
//     /* message corrupted or not intended for this recipient */
// }

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
  // <<<<<<<<<
  auto pk = std::vector<std::uint8_t>(crypto_box_PUBLICKEYBYTES);
  auto sk = std::vector<std::uint8_t>(crypto_box_SECRETKEYBYTES);
  ::crypto_box_keypair(pk.data(), sk.data());

  {
    const auto output_path = "./pk";
    auto fstream = std::ofstream{output_path, std::ios::binary};
    std::copy(cbegin(pk), cend(pk), std::ostream_iterator<char>{fstream});
  }
  // >>>>>>>>>

  if (argc != 4)
  {
    std::cerr << "Usage: " << argv[0] << "/path/to/public_key /path/to/input_file /path/to/encrypted_output_file\n";
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

  // <<<<<<<<<
  // const auto encrypted = encrypt(public_key, file);

    const auto pk_from_file = [&]
    {
      const auto file_path = "./pk";
      auto fstream = std::ifstream{file_path, std::ios::binary};
      return std::vector<std::uint8_t>{
        std::istreambuf_iterator<char>(fstream),
        std::istreambuf_iterator<char>()};
    }();

  const auto encrypted = encrypt(pk_from_file, file);
  // >>>>>>>>>

  const auto output_path = argv[3];
  auto fstream = std::ofstream{output_path, std::ios::binary};
  std::copy(cbegin(encrypted), cend(encrypted), std::ostream_iterator<char>{fstream});

  // <<<<<<<<<
  {
    auto decrypted = std::vector<std::uint8_t>(file.size());
    if (::crypto_box_seal_open(decrypted.data(), encrypted.data(), encrypted.size(), pk_from_file.data(), sk.data()) != 0)
    {
      std::cerr << "corrupted or not intended for this recipient\n";
      return 1;
    }

    // for (auto x : decrypted)
    // {
    //   std::cout << x;
    // }
    // std::cout << '\n';
      const auto output_path = "./tmp";
      auto fstream = std::ofstream{output_path, std::ios::binary};
      std::copy(cbegin(decrypted), cend(decrypted), std::ostream_iterator<char>{fstream});
  }
  // >>>>>>>>>

  return 0;
}
