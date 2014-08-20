#pragma once
/// \file
///
/// Range utitilties
#include <hm3/utility/range-v3.hpp>

namespace hm3 {

using namespace ranges;

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

}  // namespace hm3

#define HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(Value)  \
  static_assert(::hm3::RandomAccessRange<decltype(Value)>{} \
                 && ::hm3::SizedRange<decltype(Value)>{},   \
                "The value " #Value " is not a random access sized range!")
