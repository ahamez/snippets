#ifndef _SDD_INTERNAL_DD_ALPHA_HH_
#define _SDD_INTERNAL_DD_ALPHA_HH_

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <type_traits>

#include <boost/container/flat_map.hpp>

#include "sdd/conf/traits.hh"
#include "sdd/internal/dd/sdd_ptr_fwd.hh"
#include "sdd/internal/util/hash.hh"
#include "sdd/internal/util/packed.hh"

namespace sdd { namespace internal { namespace dd {

/*-------------------------------------------------------------------------------------------*/

enum class alpha_tag {general, binary_domain};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct alpha_fun;

template <typename C, typename Valuation, enum class alpha_tag>
struct alpha_impl;

template <typename C, typename Valuation, enum class alpha_tag>
struct alpha_builder_impl;

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct tag_for_valuation;

template <typename C>
struct tag_for_valuation<C, SDD_ptr<C>>
{
  static constexpr alpha_tag value = alpha_tag::general;
};

template <typename C>
struct tag_for_valuation<C, typename C::Values>
{
  static constexpr alpha_tag value = alpha_tag::binary_domain;
//                                   ? alpha_tag::binary_domain
//                                   : alpha_tag::general;
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct arc
{
  const Valuation  valuation;
  const SDD_ptr<C> successor;

  arc(Valuation&& val, SDD_ptr<C>&& succ)
    : valuation(std::move(val))
    , successor(std::move(succ))
  {
  }

  bool
  operator==(const arc& other)
  const noexcept
  {
    return valuation == other.valuation and successor == other.successor;
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct _SDD_PACKED alpha_impl<C, Valuation, alpha_tag::general>
{
  /// We don't directly use an array of arc<C, Valuation> as we don't want to have
  /// a default value for SDD_ptr<C>.
  char* arcs_;

  typedef typename C::alpha_size_type size_type;
  size_type size_;

  alpha_impl(std::size_t size)
    : arcs_(new char[sizeof(arc<C, Valuation>) * size])
    , size_(size)
  {
  }

  ~alpha_impl()
  noexcept
  {
    if (arcs_ != nullptr)
    {
      delete[] arcs_;
    }
  }

  alpha_impl(const alpha_impl&) = delete;
  alpha_impl& operator=(const alpha_impl&) = delete;

  alpha_impl(alpha_impl&& other)
    : arcs_(other.arcs_)
    , size_(other.size_)
  {
    other.arcs_ = nullptr;
    other.size_ = 0;
  }

  alpha_impl&
  operator=(alpha_impl&& other)
  {
    if (arcs_ != other.arcs_)
    {
      arcs_ = other.arcs_;
      size_ = other.size_;
      other.arcs_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  bool
  empty()
  const noexcept
  {
    return size_ == 0;
  }

  std::size_t
  size()
  const noexcept
  {
    return size_;
  }
  
  /// Specialization for alpha that we can iterate on byte per byte.
  template <typename Val = Valuation>
  // the template parameter is here to make enable_if work
  typename
  std::enable_if< std::is_same<Val, SDD_ptr<C>>::value
                  or is_byte_iterable<Val>::value
                , bool>::type
  operator==(const alpha_impl& other)
  const noexcept
  {
    return size_ == other.size_
       and ::memcmp(arcs_, other.arcs_, size_ * sizeof(arc<C, Valuation>)) == 0;
  }

  /// Specialization for values that we can't iterate on byte per byte.
  template <typename Val = Valuation>
  // the template parameter is here to make enable_if work
  typename
  std::enable_if< std::is_same<Val, typename C::Values>::value
                  and not is_byte_iterable<Val>::value
                , bool>::type
  operator==(const alpha_impl& other)
  const noexcept
  {
    return size_ == other.size_ and std::equal(begin(), end(), other.begin());
  }


  typedef const arc<C, Valuation>* const_iterator;

  const_iterator
  begin()
  const noexcept
  {
    return reinterpret_cast<const arc<C, Valuation>*>(arcs_);
  }

  const_iterator
  end()
  const noexcept
  {
    return reinterpret_cast<const arc<C, Valuation>*>(arcs_) + size_;
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct alpha_builder_impl<C, Valuation, alpha_tag::general>
{
  typedef boost::container::flat_map<Valuation, SDD_ptr<C>> container_type;
  container_type container_;
  
  typedef alpha_fun<C, Valuation> alpha_type;
  
  void
  reserve(std::size_t size)
  {
    container_.reserve(size);
  }
  
  /// \param val shall be an element of a partition, no verification will be made.
  void
  add_arc(Valuation&& val, SDD_ptr<C>&& succ)
  {
    container_.emplace(std::move(val), std::move(succ));
  }
  
  alpha_fun<C, Valuation>
  consolidate()
  {
    alpha_fun<C, Valuation> res(container_.size());
    std::size_t i = 0;
    for (auto& a : container_)
    {
      // placement new
      new (reinterpret_cast<arc<C, Valuation>*>(res.arcs_) + i)
      arc<C, Valuation>(std::move(a.first), std::move(a.second));
      ++i;
    }
    return res;
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct _SDD_PACKED alpha_impl<C, Valuation , alpha_tag::binary_domain>
{
  //  lo_  hi_
  //  --------------
  //  |0|  |0|  empty
  //   x   |0|  {0} -> x
  //  |0|   y   {1} -> y
  //   x    y   {0} -> x + {1} -> y
  //   x    x   {0,1} -> x

  typedef typename C::Values values_type;

  const SDD_ptr<C> lo_;
  const SDD_ptr<C> hi_;

  alpha_impl(SDD_ptr<C>&& lo, SDD_ptr<C>&& hi)
  noexcept
    : lo_(std::move(lo))
    , hi_(std::move(hi))
  {
  }

  alpha_impl(const SDD_ptr<C>& lo, const SDD_ptr<C>& hi)
  noexcept
    : lo_(lo)
    , hi_(hi)
  {
  }

  alpha_impl(const alpha_impl&) = delete;
  alpha_impl& operator=(const alpha_impl&) = delete;

  alpha_impl(alpha_impl&& other)
    : lo_(other.lo_)
    , hi_(other.hi_)
  {
  }

  bool
  empty()
  const noexcept
  {
    return lo_.empty() and hi_.empty();
  }

  std::size_t
  size()
  const noexcept
  {
    if (lo_.empty())
    {
      return hi_.empty() ? 0 : 1;
    }
    else if (hi_.empty())
    {
      return 1;
    }
    return lo_ == hi_ ? 1 : 2;
  }

  bool
  operator==(const alpha_impl& other)
  const noexcept
  {
    return lo_ == other.lo_ and hi_ == other.hi_;
  }

  struct const_iterator
  {
    enum class mode {empty, x, y, xy, xx};
    enum class mode mode_;
    
    enum class state {lo, hi, end};
    enum class state state_;

    const SDD_ptr<C>& lo_;
    const SDD_ptr<C>& hi_;

    const_iterator(const SDD_ptr<C>& lo, const SDD_ptr<C>& hi)
    noexcept
      : lo_(lo)
      , hi_(hi)
      , mode_()
      , state_()
    {
      if (lo_.empty())
      {
        if (hi_.empty())
        {
          mode_  = mode::empty;
          state_ = state::end;
        }
        else // not hi_.empty()
        {
          mode_  = mode::y;
          state_ = state::hi;
        }
      }
      else if (hi_.empty())
      {
        mode_  = mode::x;
        state_ = state::lo;
      }
      else
      {
        state_ = state::lo;
        if (lo_ == hi_)
        {
          mode_  = mode::xx;
        }
        else // lo_ != hi_
        {
          mode_ = mode::xy;
        }
      }
    }

    const_iterator(const SDD_ptr<C>& lo, const SDD_ptr<C>& hi, enum class state)
    noexcept
     : lo_(lo)
     , hi_(hi)
     , mode_()
     , state_(state::end)
    {
    }

    bool
    operator==(const const_iterator& other)
    const noexcept
    {
      return state_ == other.state_;
    }

    bool
    operator!=(const const_iterator& other)
    const noexcept
    {
      return state_ != other.state_;
    }

    const_iterator&
    operator++()
    noexcept
    {
      switch (mode_)
      {
          case mode::x  : // fall through
          case mode::y  : // fall through
          case mode::xx : state_ = state::end; break;
          case mode::xy :
          {
            switch (state_)
            {
              case state::lo : state_ = state::hi;  break;
              case state::hi : // fall through
              default        : state_ = state::end; break;
            }
            // fall through
          }               
          default:;
      }
    }

    arc<C, values_type>
    operator*()
    const noexcept
    {
      SDD_ptr<C> succ = lo_;
      typename C::Values values;
      switch (mode_)
      {
        case mode::x  : values.insert(false); break;
        case mode::y  : values.insert(true); succ = hi_; break;
        case mode::xx : values.insert(false); values.insert(true); break;
        case mode::xy :
        {
          switch (state_)
          {
            case state::lo : values.insert(false); break;
            case state::hi : values.insert(true); succ = hi_; break;
            default : /* state::end */;
          }
        }
        default: /* mode::empty */;
      }
      return arc<C, typename C::Values>(std::move(values), std::move(succ));
    }
    
    std::unique_ptr<arc<C, values_type>>
    operator->()
    const noexcept
    {
      if (state_ == state::end)
      {
        return std::unique_ptr<arc<C, values_type>>(nullptr);
      }
      else
      {
        return std::unique_ptr<arc<C, values_type>>(new arc<C, values_type>(operator*()));
      }
    }
  };

  const_iterator
  begin()
  const noexcept
  {
    return const_iterator(hi_, lo_);
  }

  const_iterator
  end()
  const noexcept
  {
    return const_iterator(hi_, lo_);
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct alpha_builder_impl<C, Valuation, alpha_tag::binary_domain>
{
  typedef typename C::Values values_type;
  typedef boost::container::flat_map<values_type, SDD_ptr<C>> container_type;
  container_type container_;

  typedef alpha_impl<C, values_type, alpha_tag::binary_domain> alpha_type;

  void
  reserve(std::size_t size)
  {
    container_.reserve(size);
  }

  /// \param val shall be an element of a partition, no verification will be made.
  void
  add_arc(values_type&& val, SDD_ptr<C>&& succ)
  {
    container_.emplace(std::move(val), std::move(succ));
  }

  alpha_type
  consolidate()
  {
    if (container_.size() == 0)
    {
      return alpha_type(zero<C>(), zero<C>());
    }
    else if (container_.size() == 1)
    {
      values_type false_values;
      false_values.insert(false);

      const auto diff = difference( global_context<C>()
                                  , values_type(container_.begin()->first)
                                  , std::move(false_values));
      if (diff.empty()) // only false in values
      {
        return alpha_type(zero<C>(), container_.begin()->second);
      }
      else
      {
        return alpha_type(container_.begin()->second, zero<C>());
      }
    }
    else
    {
      
    }
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct _SDD_PACKED alpha_fun
  : public alpha_impl<C, Valuation, tag_for_valuation<C, Valuation>::value>
{
  typedef alpha_impl<C, Valuation, alpha_tag::general> base_type;

  alpha_fun(std::size_t size)
    : base_type(size)
  {
  }
};

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct alpha_fun_builder
  : public alpha_builder_impl< C, Valuation, tag_for_valuation<C, Valuation>::value>
{
};

/*-------------------------------------------------------------------------------------------*/

}}} // namespace sdd::internal::dd

namespace std {

/*-------------------------------------------------------------------------------------------*/

template <typename C, typename Valuation>
struct hash<sdd::internal::dd::alpha_impl< C, Valuation
                                         , sdd::internal::dd::alpha_tag::binary_domain>>
{
  typedef sdd::internal::dd::alpha_impl< C, Valuation
                                       , sdd::internal::dd::alpha_tag::binary_domain>
          alpha_impl_type;

  std::size_t
  operator()(const alpha_impl_type& x)
  const noexcept
  {
    return 0;
  }
};

template <typename C, typename Valuation>
struct hash<sdd::internal::dd::alpha_impl<C, Valuation, sdd::internal::dd::alpha_tag::general>>
{
  typedef sdd::internal::dd::alpha_impl<C, Valuation, sdd::internal::dd::alpha_tag::general>
          alpha_impl_type;

  /// Specialization for alpha that we can iterate on byte per byte.
  template <typename Val = Valuation>
  // the template parameter is here to make enable_if work
  typename
  std::enable_if< std::is_same<Val, sdd::internal::dd::SDD_ptr<C>>::value
                or sdd::is_byte_iterable<Val>::value
               , std::size_t>::type
  operator()(const alpha_impl_type& x)
  const noexcept
  {
    std::size_t res = 0;
    sdd::internal::util::MurmurHash3_x86_32( x.arcs_
                                           , sizeof(sdd::internal::dd::arc<C, Valuation>)
                                             * x.size_
                                           , 0
                                           , &res
                                           );
    return res;
  }
  
  /// Specialization for values that we can't iterate on byte per byte.
  template <typename Val = Valuation>
  // the template parameter is here to make enable_if work
  typename
  std::enable_if< std::is_same<Val, typename C::Values>::value
                and not sdd::is_byte_iterable<Val>::value
                , std::size_t>::type
  operator()(const alpha_impl_type& x)
  const noexcept
  {
    std::size_t seed = 0;
    for (const auto& arc : x)
    {
      sdd::internal::util::hash_combine(seed, arc.valuation);
      sdd::internal::util::hash_combine(seed, arc.successor);
    }
    return seed;
  }
};

template <typename C, typename Valuation>
struct hash<sdd::internal::dd::alpha_fun<C, Valuation>>
{
  std::size_t
  operator()(const sdd::internal::dd::alpha_fun<C, Valuation>& x)
  const noexcept
  {
    return hash<typename sdd::internal::dd::alpha_fun<C, Valuation>::base_type>()(x);
  }
};

/*-------------------------------------------------------------------------------------------*/

} // namespace std

#endif // _SDD_INTERNAL_DD_ALPHA_HH_