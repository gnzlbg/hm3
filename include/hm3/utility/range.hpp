#pragma once
/// \file all.hpp Range utitilties
#include <hm3/utility/range-v3.hpp>
#include <hm3/utility/returns.hpp>

namespace hm3 {

using namespace ranges;

template <typename Box, typename Int,
          typename value_type = typename Box::value_type,
          CONCEPT_REQUIRES_(Same<Int, value_type>{}
                            and RandomAccessIncrementable<value_type>{})>
auto boxed_ints(Int const& from, Int const& to) {
  return view::iota(from, to)
         | view::transform([](value_type const& v) -> Box { return v; });
}

template <typename Box, typename value_type = typename Box::value_type,
          CONCEPT_REQUIRES_(RandomAccessIncrementable<value_type>{})>
auto boxed_ints(Box const& from, Box const& to) {
  return boxed_ints<Box>(*from, *to);
}

template <class Rng,
          CONCEPT_REQUIRES_(InputRange<Rng>{} and !ForwardRange<Rng>{})>
auto use_(Rng&& rng) HM3_RETURNS(uncvref_t<Rng>{rng});

template <class Rng, CONCEPT_REQUIRES_(ForwardRange<Rng>{})>
auto use_(Rng&& rng) HM3_RETURNS(std::forward<Rng>(rng));

}  // namespace hm3

#define HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(Value)  \
  static_assert(::hm3::RandomAccessRange<decltype(Value)>{} \
                 && ::hm3::SizedRange<decltype(Value)>{},   \
                "The value " #Value " is not a random access sized range!")
