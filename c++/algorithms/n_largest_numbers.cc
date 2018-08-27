#include <iostream>
#include <set>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

template <typename Iterator>
std::vector<typename Iterator::value_type>
find_10_largest(Iterator cit, const Iterator end)
{
  typedef typename Iterator::value_type value_type;
  std::set<value_type> res;  
  value_type min = std::numeric_limits<value_type>::min();
  
  for(cit; cit != end; ++cit)
  {
    if (*cit >= min)
    {
      if (res.size() >= 10)
      {
        res.erase(min);
        min = *res.begin();
      }
      res.insert(*cit);
    }
  }
  return std::vector<value_type>(res.begin(), res.end());
}

template <typename Iterator>
std::vector<typename Iterator::value_type>
find_10_largest_2(Iterator begin, const Iterator end)
{
  typedef typename Iterator::value_type value_type;

  std::vector<value_type> tmp(begin,end);
  const std::size_t size = std::distance(tmp.begin(), tmp.end());
  std::nth_element(tmp.begin(), tmp.begin() + size - 10, tmp.end());
  return std::vector<value_type>(tmp.begin() + size - 10, tmp.end());
}

int main ()
{
  srandom(static_cast<int>(time(NULL)));
  std::vector<unsigned int> data;
  for( size_t i = 0; i < 50; ++i)
  {
    data.push_back(static_cast<unsigned int>(random() % 100));
  }

  for(std::vector<unsigned int>::const_iterator cit = data.begin(); cit != data.end(); ++cit)
  {
    std::cout << *cit << "|";
  }
  std::cout << std::endl << std::endl;
  
  ///////////////////////////////////////////////////////////////////////
  typedef std::vector<unsigned int> res_type;

  ///////////////////////////////////////////////////////////////////////
  const res_type res1 = find_10_largest(data.begin(), data.end());
  for( res_type::const_iterator cit = res1.begin(); cit != res1.end(); ++cit)
  {
    std::cout << *cit << "|";
  }
  std::cout << std::endl;

  ///////////////////////////////////////////////////////////////////////
  const res_type res2 = find_10_largest_2(data.begin(), data.end());
  for( res_type::const_iterator cit = res2.begin(); cit != res2.end(); ++cit)
  {
    std::cout << *cit << "|";
  }
  std::cout << std::endl;

  return 0;
}