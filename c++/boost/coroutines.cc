#include <functional> // bind
#include <iostream>

#include <boost/coroutine/asymmetric_coroutine.hpp>

template <typename T>
using pull_type = typename boost::coroutines::asymmetric_coroutine<T>::pull_type;

template <typename T>
using push_type = typename boost::coroutines::asymmetric_coroutine<T>::push_type;

template <typename T>
void pusher(push_type<T>& yield, T start, T end)
{
  for(T i = start; i < end; ++i)
  {
    yield(i);
  }
};

template <typename T>
pull_type<T>
generator(T start, T end)
{
  return pull_type<T>(std::bind(pusher<T>, std::placeholders::_1, start, end));
}

int main()
{
  // for(auto i : generator(1, 10))
  // {
  //   std::cout << i <<  " ";
  // }

  auto gen = generator(1, 10);
  // while (gen)
  // {
  //   auto i = gen.get();
  //   gen();
  //   std::cout << i << " ";
  // }

  auto it = std::begin(gen);
  auto end =  std::end(gen);
  for (; it != end; ++it)
  {
    std::cout << *it << " ";
  }

  return 0;
}
