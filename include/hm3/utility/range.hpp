#pragma once
/// \file
///
/// Range utitilties
#include <hm3/utility/range-v3.hpp>

namespace ranges {
inline namespace v3 {
template <typename T, typename U,
          CONCEPT_REQUIRES_(EqualityComparable<T>{}
                            and EqualityComparable<U>{})>
bool operator==(compressed_pair<T, U> const& t,
                compressed_pair<T, U> const& u) {
  return t.first() == u.first() and t.second() == u.second();
}

template <typename T, typename U,
          CONCEPT_REQUIRES_(EqualityComparable<T>{}
                            and EqualityComparable<U>{})>
bool operator!=(compressed_pair<T, U> const& t,
                compressed_pair<T, U> const& u) {
  return !(t == u);
}

template <typename OStream, typename T, typename U>
OStream& operator<<(OStream& os, compressed_pair<T, U> const& t) {
  os << "(" << t.first() << ", " << t.second() << ")";
  return os;
}

}  // inline namespace v3
}  // namespace ranges

namespace hm3 {

template <typename T, typename U>
using pair = ranges::compressed_pair<T, U>;

template <typename T, typename U>
auto make_pair(T&& t, U&& u) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
 ranges::make_compressed_pair(std::forward<T>(t), std::forward<U>(u)));

template <typename T>
using static_const = ranges::static_const<T>;

// namespace ranges = ranges::v3;
namespace view   = ranges::view;
namespace action = ranges::action;

using ranges::uncvref_t;
using ranges::iterator_value_t;
using ranges::value_type;
using ranges::difference_type;
using ranges::range_iterator_t;
using ranges::unwrap_reference;
using ranges::tuple_indices_t;
using ranges::range_value_t;

// standard concepts:
using ranges::Same;

using ranges::Range;
using ranges::InputRange;
using ranges::ForwardRange;
using ranges::RandomAccessRange;
using ranges::IteratorRange;
using ranges::InputIterator;
using ranges::ForwardIterator;
using ranges::RandomAccessIterator;
using ranges::RandomAccessIncrementable;
using ranges::Integral;
using ranges::SignedIntegral;
using ranges::UnsignedIntegral;
using ranges::WeaklyOrdered;
using ranges::EqualityComparable;
using ranges::TotallyOrdered;
using ranges::Regular;
using ranges::SemiRegular;
using ranges::Movable;
using ranges::Copyable;
using ranges::Container;
using ranges::Constructible;
using ranges::CopyConstructible;
using ranges::DefaultConstructible;
using ranges::MoveConstructible;
using ranges::Function;
using ranges::ConvertibleTo;

// core algorithms
using ranges::begin;
using ranges::end;
using ranges::cbegin;
using ranges::cend;
using ranges::front;
using ranges::back;
using ranges::at;
using ranges::size;
using ranges::equal;

// some ranges
using ranges::iterator_range;

/// Creates a range of boxed integers (e.g. boxed in a compact_optional or
/// bounded_integer type)
template <typename Box, typename Int,
          typename value_type = typename Box::value_type,
          CONCEPT_REQUIRES_(Same<Int, value_type>{}
                            and RandomAccessIncrementable<value_type>{})>
auto boxed_ints(Int const& from, Int const& to) {
  return view::iota(from, to)
         | view::transform([](value_type const& v) -> Box { return v; });
}

/// Creates a range of boxed integers (e.g. boxed in a compact_optional or
/// bounded_integer type)

template <typename Box, typename value_type = typename Box::value_type,
          CONCEPT_REQUIRES_(RandomAccessIncrementable<value_type>{})>
auto boxed_ints(Box const& from, Box const& to) {
  return boxed_ints<Box>(*from, *to);
}

/// Returns a copy of a range if its SinglePass, and otherwise it returns the
/// range (useful when one want's to iterate over a single pass range multiple
/// times)
template <typename Rng,
          CONCEPT_REQUIRES_(InputRange<Rng>{} and !ForwardRange<Rng>{})>
auto use_copy_if_single_pass(Rng&& rng) noexcept {
  return uncvref_t<Rng>{rng};
}

/// Returns a copy of a range if its SinglePass, and otherwise it returns the
/// range (useful when one want's to iterate over a single pass range multiple
/// times)
template <typename Rng, CONCEPT_REQUIRES_(ForwardRange<Rng>{})>
auto use_copy_if_single_pass(Rng&& rng) noexcept {
  return std::forward<Rng>(rng);
}

struct tuple_for_each_indexed_fn {
 private:
  template <typename Tup, typename Fun, std::size_t... Is>
  static void impl(Tup&& tup, Fun fun, meta::index_sequence<Is...>) {
    (void)std::initializer_list<int>{
     (void)fun(Is, std::get<Is>(std::forward<Tup>(tup)))...};
  }

 public:
  template <typename Tup, typename Fun>
  Fun operator()(Tup&& tup, Fun fun) const {
    tuple_for_each_indexed_fn::impl(std::forward<Tup>(tup), std::ref(fun),
                                    tuple_indices_t<Tup>{});
    return fun;
  }
};

namespace {
constexpr auto&& tuple_for_each_indexed
 = static_const<tuple_for_each_indexed_fn>::value;
}  // namespace

}  // namespace hm3

#define HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(Value)   \
  static_assert(ranges::RandomAccessRange<decltype(Value)>{} \
                 && ranges::SizedRange<decltype(Value)>{},   \
                "The value " #Value " is not a random access sized range!")
