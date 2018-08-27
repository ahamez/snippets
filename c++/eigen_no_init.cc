#include <vector>
#include <iostream>
#include <Eigen/Geometry>

int
main ()
{
  auto a = std::vector<char>(1024, 42);
  const auto* f1 = new (a.data()) Eigen::Vector3d{};
  std::cout << *f1 << "\n";
}