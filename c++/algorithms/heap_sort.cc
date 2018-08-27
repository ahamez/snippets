#include <algorithm>
//#include <functional>
#include <iostream>
#include <vector>

#include <tr1/array>

#include <cassert>
#include <cstdlib>
#include <ctime>

typedef std::tr1::array<unsigned int,7> array_type;
typedef std::vector<unsigned int> vec_type;

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
std::ostream& operator<<(std::ostream&, const array_type&);

std::ostream&
operator<<(std::ostream& os, const array_type& c)
{
  for( array_type::const_iterator cit = c.begin(); cit != c.end(); ++cit)
  {
    os << *cit << "|";
  }
//  for(auto i : c)
//  {
//    os << i << "|";
//  }
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream&, const vec_type&);

std::ostream&
operator<<(std::ostream& os, const vec_type& c)
{
  for( vec_type::const_iterator cit = c.begin(); cit != c.end(); ++cit)
  {
    os << *cit << "|";
  }
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
//template<typename RandomAccessIterator>
//inline
//RandomAccessIterator
//parent(const RandomAccessIterator first, const RandomAccessIterator cit)
//{
//  return first + (std::distance(first, cit) / 2);
//}

////////////////////////////////////////////////////////////////////////////////////
template<typename RandomAccessIterator>
inline
RandomAccessIterator
left(const RandomAccessIterator first, const RandomAccessIterator cit)
{
  return first + ((std::distance(first, cit)+1) * 2) - 1; // -1 because we had to shift the index by +1
}

////////////////////////////////////////////////////////////////////////////////////
template<typename RandomAccessIterator>
inline
RandomAccessIterator
right(const RandomAccessIterator first, const RandomAccessIterator cit)
{
  return first + ((std::distance(first, cit)+1) * 2); // -1 because we had to shift the index by +1
}

////////////////////////////////////////////////////////////////////////////////////
template<typename RandomAccessIterator>
void
heapify(RandomAccessIterator first, const RandomAccessIterator last, const RandomAccessIterator i, int depth = 0)
{
  const RandomAccessIterator l = left(first, i);
  const RandomAccessIterator r = right(first, i);
  RandomAccessIterator max = i;

//  std::cout << std::endl;
//  std::cout << tab(depth) << vec_type(first, last) << std::endl;
//  std::cout << tab(depth) << "*i=" << *i << " | *l=" << *l << " | *r=" << *r << std::endl;
  
  if (l < last and *l > *i) // left son is greater
  {
    max = l;
  }
  
  if (r < last and *r > *max) // right son is greater
  {
    max = r;
  }

  if (max != i)
  {
    std::swap(*i, *max);
    heapify(first, last, max, depth + 4);
  }
}

////////////////////////////////////////////////////////////////////////////////////
template<typename RandomAccessIterator>
void
heapify_iterative(RandomAccessIterator first, const RandomAccessIterator last, RandomAccessIterator i)
{
  RandomAccessIterator max = i;

  while (true)
  {
    const RandomAccessIterator l = left(first, i);
    const RandomAccessIterator r = right(first, i);

    if (l < last and *l > *i) // left son is greater
    {
//      std::cout << "l < last and *l > *i" << std::endl;
      max = l;
    }
    
    if (r < last and *r > *max) // right son is greater
    {
//      std::cout << "r < last and *r > *i" << std::endl;
      max = r;
    }
  
    if (max != i) 
    {
      std::swap(*i, *max);
      i = max;
    }
    else
    {
      break;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////
template <typename RandomAccessIterator>
void
make_heap(const RandomAccessIterator first, const RandomAccessIterator last)
{
  for (std::size_t i = std::distance(first, last) / 2 ; i > 0; --i)
  {
    heapify(first, last, first + i - 1);
  }
}

////////////////////////////////////////////////////////////////////////////////////
template<typename RandomAccessIterator>
void
heap_sort(RandomAccessIterator first, RandomAccessIterator last)
{
  ::make_heap(first, last);
  for (std::size_t i = std::distance(first, last) - 1; i >= 1; --i)
  {
    std::swap(*first, *(first + i));
    --last;
    heapify(first, last, first);
  }
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition ( BidirectionalIterator first,
                                 BidirectionalIterator last, Predicate pred )
{
  while (true)
  {
    while (first!=last && pred(*first)) ++first;
    if (first==last--) break;
    while (first!=last && !pred(*last)) --last;
    if (first==last) break;
    std::swap (*first++,*last);
  }
  return first;
}

struct comp
{
  const unsigned int rhs;
  
  comp(unsigned int r):rhs(r) {}
  
  bool
  operator()(unsigned int lhs)
  const
  {
    return lhs < rhs;
  }
};

//template<typename RandomAccessIterator>
void
foo(std::vector<unsigned int>::iterator first, std::vector<unsigned int>::iterator last)
{
  const std::size_t dist = std::distance(first, last);
  if (dist >= 1)
  {
    const std::vector<unsigned int>::iterator middle = first + dist/2;
//    using namespace boost::lambda;
//    std::partition(first, last, _1 < *middle);
//    std::partition(first, last, std::bind2nd(std::less<std::size_t>(), *middle));
//    ::partition(first, last, std::bind2nd(std::less<std::size_t>(), *middle));
    ::partition(first, last, comp(*middle));
    foo(first, middle);
    foo(middle, last);
  }
}


////////////////////////////////////////////////////////////////////////////////////
int
main ()
{
//  array_type a1 = {16,4,10,14,7,9,3,2,8,1};
//  array_type a1 = {4,10,15,17,0,18,15};
//  array_type a2 = a1;

//  std::cout << "a    = " << a1 << std::endl;
//  ::heap_sort(a1.begin(), a1.end());
//  std::cout << std::endl << "::  = " << a1 << std::endl;
//  std::make_heap(a2.begin(), a2.end());
//  std::cout << "std = " << a2 << std::endl;
//  std::cout << std::endl;

  
  srandom(static_cast<int>(time(NULL)));
  std::vector<unsigned int> data;
  for(std::size_t i = 0; i < 50; ++i)
  {
    for( unsigned int i = 0; i < 1000; ++i)
    {
      data.push_back(static_cast<unsigned int>(random() % 20));
    }
    std::vector<unsigned int> oracle(data.begin(), data.end());
//    std::cout << "ref::" << data << std::endl;
//    heap_sort(data.begin(), data.end());
    foo(data.begin(), data.end());
    std::sort(oracle.begin(), oracle.end());
//    std::cout << "   ::" << data << std::endl;
//    std::cout << "std::" << oracle << std::endl;
    assert(data == oracle);
  }
  std::cout << "OK" << std::endl;
  
  return 0;
}