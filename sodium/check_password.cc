#include <iostream>
#include <string>

#include <sodium.h>

// Return true if password is OK.
bool
check_password(const std::string& hash, const std::string& password)
{
  const auto res = crypto_pwhash_str_verify(
    hash.c_str(),
    password.c_str(),
    password.size()
  );

  return res == 0;
}

int main()
{
  // Compute hash of a password. To be written in a different tool.
  const auto hash =
  []{
    const auto password = std::string{"FOOBAR"};

    auto hash = std::string(crypto_pwhash_STRBYTES, ' ');
    const auto ret = crypto_pwhash_str(
      &hash[0],
      password.c_str(),
      password.size(),
      crypto_pwhash_OPSLIMIT_INTERACTIVE,
      crypto_pwhash_MEMLIMIT_INTERACTIVE);

    if (ret != 0)
    {
      throw std::runtime_error{"Cannot compute hash"};
    }

    return hash;
  }();

  std::cout << hash << '\n';


  std::cout << std::boolalpha << check_password(hash, "FOOBAR") << '\n';
  std::cout << std::boolalpha << check_password(hash, "FOOBARZ") << '\n';

  return 0;
}
