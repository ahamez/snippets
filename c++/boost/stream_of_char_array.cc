#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>

using array_stream = boost::iostreams::stream<boost::iostreams::array_source>;

int main ()
{
  const auto str = "abc";
  auto&& in = array_stream{str, str+3};
  std::string name;
  std::getline(in, name);
  std::cout << name << '\n';
  return 0;
}