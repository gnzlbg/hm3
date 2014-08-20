#pragma once
/// \file range_v3.hpp Includes the range-v3 library

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <range/v3/all.hpp>
#pragma clang diagnostic pop
#include <boost/container/vector.hpp>

/// Extensions:
namespace ranges {

template <class T, std::size_t capacity>
struct is_view<boost::container::static_vector<T, capacity>> : std::false_type {
};

template <class T, class Alloc>
struct is_view<boost::container::vector<T, Alloc>> : std::false_type {};

/// TODO: still needed?
namespace ext {

struct binary_search_fn {
  /// \brief function template \c binary_search_fn::operator()
  ///
  /// range-based version of the \c binary_search std algorithm
  ///
  /// \pre `Rng` is a model of the `Range` concept
  template <typename I, typename S, typename V2, typename C = ordered_less,
            typename P                                      = ident,
            CONCEPT_REQUIRES_(IteratorRange<I, S>()
                              && BinarySearchable<I, V2, C, P>())>
  optional<I> operator()(I begin, S end, V2 const& val, C pred = C{},
                         P proj = P{}) const {
    begin = lower_bound(std::move(begin), end, val, pred, proj);
    auto&& ipred = as_function(pred);
    auto&& iproj = as_function(proj);
    return begin != end && !ipred(val, iproj(*begin)) ? optional<I>{begin}
                                                      : optional<I>{};
  }

  /// \overload
  template <typename Rng, typename V2, typename C = ordered_less,
            typename P = ident, typename I = range_iterator_t<Rng>,
            CONCEPT_REQUIRES_(Range<Rng>() && BinarySearchable<I, V2, C, P>())>
  optional<I> operator()(Rng&& rng, V2 const& val, C pred = C{},
                         P proj = P{}) const {
    static_assert(!is_infinite<Rng>::value,
                  "Trying to binary search an infinite range");
    return (*this)(begin(rng), end(rng), val, std::move(pred), std::move(proj));
  }
};

/// \sa `binary_search_fn`
/// \ingroup group-algorithms
namespace {
constexpr auto&& binary_search
 = static_const<with_braced_init_args<binary_search_fn>>::value;
}

}  // namespace ext

}  // namespace ranges
