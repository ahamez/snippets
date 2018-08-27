#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <stack>
#include <utility>

#include <tr1/array>

#include <boost/lambda/lambda.hpp>

#include <cassert>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////////
std::string tab(int);

std::string
tab(int size)
{
  std::string res;
  for( std::size_t i = 0; i < size; ++i)
  {
    res += " ";
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<unsigned int> vec_type;

std::ostream& operator<<(std::ostream&, const std::vector<unsigned int>&);

std::ostream&
operator<<(std::ostream& os, const std::vector<unsigned int>& c)
{
  std::cout << "{";
  std::vector<unsigned int>::const_iterator cit = c.begin();
  if (cit != c.end())
  {
    os << *cit;
    for( ++cit; cit != c.end(); ++cit)
    {
      os << "," << *cit;
    }
  }
  return os << "}";
}

////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
void
quicksort(const Iterator first, const Iterator last)
{
  const std::size_t distance = std::distance(first, last);
  if (distance > 1)
  {
    Iterator pivot = first + distance/2;
    std::swap(*pivot, *(last - 1)); // backup pivot in last position
    pivot = last - 1;
    const Iterator middle = std::partition(first, pivot, boost::lambda::_1 < *pivot); // partition of [first,pivot)
    std::swap(*middle, *pivot);
    quicksort(first, middle);
    quicksort(middle + 1, last);
  }
}

////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
void
quicksort_stack(const Iterator first, const Iterator last)
{
  typedef typename Iterator::value_type value_type;
  typedef std::pair<Iterator, Iterator> work_type;
  typedef std::stack< work_type > stack_type;
  stack_type stack;
  
  stack.push(make_pair(first, last));
  
  while (not stack.empty())
  {
    const work_type w = stack.top();
    stack.pop();

    const std::size_t distance = std::distance(w.first, w.second);
    if (distance > 1)
    {
      Iterator pivot = w.first + distance/2;
      std::swap(*pivot, *(w.second - 1)); // backup pivot in last position
      pivot = w.second - 1;
      const Iterator middle = std::partition(w.first, pivot, boost::lambda::_1 < *pivot); // partition of [first,pivot)
      std::swap(*middle, *pivot);
      stack.push(make_pair(w.first, middle));
      stack.push(make_pair(middle + 1, w.second));
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
void
quicksort2(Iterator first, Iterator last, int depth = 0)
{
  while (last - first > 1)
  {
    const std::size_t distance = std::distance(first, last);
    Iterator pivot = first + distance/2;
    std::swap(*pivot, *(last - 1)); // backup pivot in last position
    pivot = last - 1;
    const Iterator middle = std::partition(first, pivot, boost::lambda::_1 < *pivot); // partition of [first,pivot)
    std::swap(*middle, *pivot);
    if (middle - first < last - middle-1)
    {
      quicksort2(first, middle, depth + 4);
      first = middle + 1;
    }
    else
    {
      quicksort2(middle + 1, last, depth + 4);
      last = middle;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
int
main ()
{
  std::tr1::array<unsigned int, 4> a = {19,16,1,13};
  std::vector<unsigned int> d(a.begin(), a.end());
  quicksort(d.begin(), d.end());
  
  std::cout << std::endl << d << std::endl;
  
  srandom(static_cast<int>(time(NULL)));
  struct timeval begin;
  struct timeval end;
  ::gettimeofday(&begin, NULL);
  for(std::size_t i = 0; i < 10; ++i)
  {
    std::vector<unsigned int> data;
    for( unsigned int i = 0; i < 10000; ++i)
    {
      data.push_back(static_cast<unsigned int>(random() % 20));
    }
//    std::cout << std::endl << data << std::endl;
    std::vector<unsigned int> oracle(data.begin(), data.end());
    quicksort(data.begin(), data.end());
    std::sort(oracle.begin(), oracle.end());
//    std::cout << data << std::endl;
    assert(data == oracle);
  }  
  ::gettimeofday(&end, NULL);  
  std::cout << (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000 << "ms " << std::endl;

  return 0;
}