#pragma once
////////////////////////////////////////////////////////////////////////////////
// \file Range utilities
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <boost/range.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/algorithm/cxx11/copy_n.hpp>
#include <boost/algorithm/cxx11/iota.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <boost/range/numeric.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/any_range.hpp>
#include <boost/range/join.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <hom3/core/returns.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// Range utilities
namespace ranges {

namespace detail {

/// Range<T> is a counting range, and Range<T*> is an iterator range
template <class T> struct range {
  using type = boost::iterator_range<boost::counting_iterator<T>>;
};
template <class T> struct range<T*> { using type = boost::iterator_range<T*>; };
}  // namespace detail

/// ranges
template <class T> using range = typename detail::range<T>::type;

template <class R, class T, class U>
inline constexpr auto make(T from, U to) noexcept
 -> std::enable_if_t<!std::is_same<T, U>::value, range<R>> {
  return range<R>(R{std::move(from)}, R{std::move(to)});
}

template <class R, class T, class U>
inline constexpr auto make(T from, U to) noexcept
 -> std::enable_if_t<std::is_same<T, U>::value && !std::is_same<T, R>::value,
                     range<R>> {
  return range<R>(R{std::move(from)}, R{std::move(to)});
}

template <class T, class U>
inline constexpr auto make(T from, U to) noexcept
 -> std::enable_if_t<std::is_same<T, U>::value, range<T>> {
  return range<T>(std::move(from), std::move(to));
}

template <class T> inline constexpr range<T> irng(T b, T e) {
  return make<T>(b, e);
}

template <class T>
using any_range
 = typename boost::any_range<T, boost::forward_traversal_tag, T, Int>;

template <class T, class R> const any_range<T> anyfy(const R& rng) {
  return {rng};
}

template <class stdfun, class Predicate>
auto make_filter(Predicate p)
 RETURNS(boost::range_detail::filter_holder<stdfun>{stdfun{std::move(p)}});

template <class stdfun, class Predicate>
auto make_transform(Predicate p)
 RETURNS(boost::range_detail::transform_holder<stdfun>{stdfun{std::move(p)}});

/// Range filter
template <class T>
using range_filter
 = decltype(boost::adaptors::filtered(std::function<bool(T)>()));

namespace detail {

/// Filtered range implementation (works on RangeImpl)
template <class T> struct filtered_range {
  using type = boost::filtered_range<std::function<bool(T)>, const range<T>>;
};
template <class T> struct filtered_range<T*> {  // removes the pointer!
  using type = boost::filtered_range<std::function<bool(T)>, const range<T*>>;
};

}  // namespace detail

/// Filtered range
template <class T>
using filtered_range = typename detail::filtered_range<T>::type;

/// Filtered^2 range
template <class T>
using filtered_range2
 = boost::filtered_range<std::function<bool(T)>, const filtered_range<T>>;

/// Transformed range
template <class From, class To>
using range_transformer
 = decltype(boost::adaptors::transformed(std::function<To(From)>()));

// brief Range algorithms (i.e. all Boost.Range algorithms)
// makes all Boost.Range algorithms available
using namespace boost::algorithm;
using namespace boost::range;
using namespace boost::adaptors;

using boost::distance;
using boost::accumulate;

/// Variadic join Ranges... rs -> Range
template <class R> auto join(R&& r) RETURNS(boost::make_iterator_range(r));
template <class R1, class R2, class... Rs>
auto join(R1&& r1, R2&& r2, Rs&&... rs) RETURNS(
 boost::join(boost::join(boost::make_iterator_range(std::forward<R1>(r1)),
                         boost::make_iterator_range(std::forward<R2>(r2))),
             join(std::forward<Rs>(rs)...)));

/// Variadic zip: Ranges... rs -> Range
template <class... R>
auto zip(R&&... r) RETURNS(boost::make_iterator_range(
 boost::make_zip_iterator(boost::make_tuple(std::begin(std::forward<R>(r))...)),
 boost::make_zip_iterator(boost::make_tuple(std::end(std::forward<R>(r))...))));

}  // namespace range

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace range_detail {  // (Note: adding stuff to range_detail is pretty ugly)

/// Negates range filters:
template <typename T>
auto operator!(filter_holder<T> const& f)RETURNS(
 adaptors::filtered(std::not1(f.val)));

}  // namespace range_detail
}  // namespace boost
////////////////////////////////////////////////////////////////////////////////
