#include <algorithm>
#include <iostream>
#include <vector>

#include <tbb/tbb.h>
#include <tbb/task.h>
#include <tbb/scalable_allocator.h>
#include "tbb/parallel_sort.h"

#include <cassert>
#include <cstdlib>
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
std::ostream& operator<<(std::ostream&, const std::vector<unsigned int>&);

std::ostream&
operator<<(std::ostream& os, const std::vector<unsigned int>& c)
{
  for( std::vector<unsigned int>::const_iterator cit = c.begin(); cit != c.end(); ++cit)
  {
    os << *cit << "|";
  }
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
//inplace_merge

template <typename BidirectionalIterator>
void
merge_sort_serial(BidirectionalIterator first, BidirectionalIterator last, int depth = 0)
{
  const std::size_t size = std::distance(first, last);

  if (size <= 1)
  {
    return;
  }

  const BidirectionalIterator middle = first + size / 2;

  merge_sort_serial(first, middle, depth+2);
  merge_sort_serial(middle, last, depth+2);

  std::inplace_merge(first, middle, last);
}

////////////////////////////////////////////////////////////////////////////////////
template<typename value_type>
struct sort_task_continuation
: public tbb::task
{
  typedef std::vector<value_type, tbb::scalable_allocator<value_type> > vec_type;
  vec_type* v;
  vec_type  l;
  vec_type  r;

  template<typename Iterator>
  sort_task_continuation(vec_type* v, Iterator begin, Iterator middle, Iterator end)
  : v(v)
  , l(begin, middle)
  , r(middle, end)
  {
  }

  tbb::task*
  execute()
  {
    typename vec_type::const_iterator lcit = l.begin();
    typename vec_type::const_iterator lend = l.end();
    typename vec_type::const_iterator rcit = r.begin();
    typename vec_type::const_iterator rend = r.end();
    typename vec_type::iterator out = v->begin();

    while(lcit != lend and rcit != rend)
    {
      if (*lcit < *rcit)
      {
        *out++ = *lcit++;
      }
      else if (*lcit > *rcit)
      {
        *out++ = *rcit++;
      }
      else
      {
        *out++ = *lcit++;
        *out++ = *rcit++;
      }
    }

    // handle what possibly remains in left_res xor right_res
    assert(not(lcit != lend and rcit != rend));
    std::copy(lcit, lend, out);
    std::copy(rcit, rend, out);

    return NULL;
  }

};

template<typename value_type>
class sort_task
  : public tbb::task
{
private:

  typedef std::vector<value_type, tbb::scalable_allocator<value_type> > vec_type;
  vec_type* v;

public:

  sort_task(vec_type* v)
  : v(v)
  {
  }

  tbb::task*
  execute()
  {
    const std::size_t size = v->size();

    if (size <= 1000)
    {
      merge_sort_serial(v->begin(), v->end());
      return NULL;
    }
    else
    {
      const typename vec_type::iterator middle = v->begin() + size / 2;

      sort_task_continuation<value_type>& cont =
        *new(tbb::task::allocate_continuation()) sort_task_continuation<value_type>(v, v->begin(), middle, v->end());

      sort_task& ltask = *new(cont.allocate_child()) sort_task(&cont.l);

      this->recycle_as_child_of(cont);
      this->v = &cont.r;

      cont.set_ref_count(2);
      tbb::task::spawn(ltask);
      return this;
    }
  }

};

template<typename Iterator>
void
merge_sort(Iterator first, const Iterator last)
{
  typedef typename Iterator::value_type value_type;
  typedef std::vector<value_type, tbb::scalable_allocator<value_type> > vec_type;

  vec_type v(first, last);

  if (v.size() <= 1000)
  {
    merge_sort_serial(v.begin(), v.end());
  }
  else
  {
    sort_task<value_type>& t = *new(tbb::task::allocate_root()) sort_task<value_type>(&v);
    tbb::task::spawn_root_and_wait(t);
  }

  std::copy(v.begin(), v.end(), first);
}

template<typename Iterator>
void
merge_sort(const Iterator first, const Iterator last, bool)
{
  typedef typename Iterator::value_type value_type;
  typedef std::vector<value_type, tbb::scalable_allocator<value_type> > vec_type;
  vec_type v(first, last);
  merge_sort_serial(&v);
  std::copy(v.begin(), v.end(), first);
}


////////////////////////////////////////////////////////////////////////////////////
int main (int argc, const char * argv[])
{
  srandom(static_cast<int>(time(NULL)));
  std::vector<unsigned int> data;

  struct timeval begin;
  struct timeval end;
  ::gettimeofday(&begin, NULL);
  for(std::size_t i = 0; i < 50; ++i)
  {
    for( unsigned int i = 0; i < 10000; ++i)
    {
      data.push_back(static_cast<unsigned int>(random() % 20));
    }
    std::vector<unsigned int> oracle(data.begin(), data.end());
    merge_sort(data.begin(), data.end());
//    std::cout << data << std::endl;

    //    tbb::parallel_sort(data.begin(), data.end());
//    std::sort(data.begin(), data.end());
//    std::sort(oracle.begin(), oracle.end());
//    assert(data == oracle);
  }
  ::gettimeofday(&end, NULL);

  std::cout << (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000 << "ms " << std::endl;

  return 0;
}
