#include <algorithm>
#include <iostream>
#include <vector>

#include <cassert>
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
void
merge_sort_iterative(const Iterator begin, const Iterator end)
{
  typedef typename Iterator::value_type value_type;
  typedef std::vector<value_type> vec_type;

  const std::size_t size = std::distance(begin, end);
  std::size_t m = 1;

  while (m < size)
  {
    std::size_t i = 0;
    while (i < size - m)
    {
      vec_type l(begin + i, begin + i + m);
      vec_type r(begin + i + m, begin + std::min(i + 2*m, size));

      Iterator out = begin + i;

      typename vec_type::iterator lcit = l.begin();
      const typename vec_type::iterator lend = l.end();
      typename vec_type::iterator rcit = r.begin();
      const typename vec_type::iterator rend = r.end();

      while (lcit != lend and rcit != rend)
      {
        if (*lcit < *rcit)
        {
          *out = *lcit;
          ++out;
          ++lcit;
        }
        else
        {
          *out = *rcit;
          ++out;
          ++rcit;
        }
      }

      // handle what possibly remains in left_res xor right_res
      assert(not(lcit != lend and rcit != rend));
      std::copy(lcit, lend, out);
      std::copy(rcit, rend, out);

      i += 2*m;
    }
    m *= 2;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename value_type>
void
merge_sort_helper(std::vector<value_type>& v)
{
  typedef std::vector<value_type> vec_type;

  const std::size_t size = v.size();

  if (size <= 1)
  {
    return;
  }

  const typename std::vector<value_type>::iterator middle = v.begin() + size / 2;

  vec_type left(v.begin(), middle); // open interval [v.begin(), middle)
  vec_type right(middle, v.end());

  merge_sort_helper(left);
  merge_sort_helper(right);

  typename vec_type::const_iterator lcit = left.begin();
  typename vec_type::const_iterator lend = left.end();
  typename vec_type::const_iterator rcit = right.begin();
  typename vec_type::const_iterator rend = right.end();

  v.clear();
  while(lcit != lend and rcit != rend)
  {
    if (*lcit < *rcit)
    {
      v.push_back(*lcit);
      ++lcit;
    }
    else /*if (*lcit > *rcit)*/
    {
      v.push_back(*rcit);
      ++rcit;
    }
//    else
//    {
//      v.push_back(*lcit);
//      v.push_back(*rcit);
//      ++lcit;
//      ++rcit;
//    }
  }

  // handle what possibly remains in left_res xor right_res
  assert(not(lcit != lend and rcit != rend));
  std::copy(lcit, lend, std::inserter(v, v.end()));
  std::copy(rcit, rend, std::inserter(v, v.end()));

}

template< typename Iterator>
void
merge_sort(Iterator begin, const Iterator end)
{
  typedef typename Iterator::value_type value_type;
  std::vector<value_type> v(begin, end);
  merge_sort_helper<value_type>(v);
  std::copy(v.begin(), v.end(), begin);
}

//////////////////////////////////////////////////////////////////////////////////////////////
int
main ()
{
  srandom(static_cast<int>(time(NULL)));
  std::vector<unsigned int> data;
  for( unsigned int i = 0; i < 100; ++i)
  {
    data.push_back(static_cast<unsigned int>(random() % 20));
//    data.push_back(i);
  }
  std::random_shuffle(data.begin(), data.end());
  std::vector<unsigned int> data2(data.begin(), data.end());

  for(std::vector<unsigned int>::const_iterator cit = data.begin(); cit != data.end(); ++cit)
  {
    std::cout << *cit << "|";
  }
  std::cout << std::endl << std::endl;

//  merge_sort(data.begin(), data.end());
//  for(std::vector<unsigned int>::const_iterator cit = data.begin(); cit != data.end(); ++cit)
//  {
//    std::cout << *cit << "|";
//  }
//  std::cout << std::endl << std::endl;

  merge_sort_iterative(data.begin(), data.end());
  for(std::vector<unsigned int>::const_iterator cit = data.begin(); cit != data.end(); ++cit)
  {
    std::cout << *cit << "|";
  }
  std::cout << std::endl << std::endl;


  std::sort(data2.begin(), data2.end());
  assert( data == data2);


  return 0;
}
