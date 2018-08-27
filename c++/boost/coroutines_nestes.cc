#include <functional>
#include <iostream>
#include <boost/coroutine/asymmetric_coroutine.hpp>

using generator_t = typename boost::coroutines::asymmetric_coroutine<int>::pull_type;
using yield_t     = typename boost::coroutines::asymmetric_coroutine<int>::push_type;

void iterator(yield_t& iterator_yield, yield_t& network_yield)
{
 for (auto i = 0; i < 5; ++i)
 {
   if (i % 2 == 0) network_yield(i);
   iterator_yield(i);
 }
}

void loop(yield_t& network_yield)
{
 // bind iterator's second parameter to network_yield function with std::bind
 const auto fn = std::bind(iterator, std::placeholders::_1, std::ref(network_yield));
 // or with a lambda
 // const auto fn = [&](yield_t& iterator_yield){iterator(iterator_yield, network_yield);};
 for (auto i : generator_t{fn})
 {
   std::cout << "loop " << i << '\n';
 }
}

int main()
{
 for (auto i : generator_t{loop}) // loop on network
 {
   std::cout << "main " << i << '\n';
 }
 return 0;
};
