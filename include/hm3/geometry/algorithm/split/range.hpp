#pragma once
/// \file
///
/// Split range.
#include <hm3/utility/small_vector.hpp>

namespace hm3::geometry {

namespace split_range_detail {

struct split_range_fn {
  // Split each primitive in vector \p v against each primitive in range \p
  // rng (and the result is returned in the vector \p v) using the split
  // function \p sf.
  template <typename Vec, typename Rng, typename SplitF>
  static auto ranges(Vec& v, Rng&& rng, SplitF&& sf) {
    HM3_ASSERT(not v.empty(), "");
    for (auto&& p : rng) {
      for (auto&& it = begin(v); it != end(v);) {
        // Split the current primitive with the first primitive p that works
        auto r = sf(*it, p);

        // If the result of the split is zero or one polygon, the poly line does
        // not split the current polygon so go to the next one:
        if (r.size() < 2_su) {
          ++it;
          continue;
        }

        // If the split succeeded, replace the split polygon with the two
        // resulting polygons while maintaining the iterators valid:
        v.erase(it);
        std::size_t pos = it - begin(v);
        v.insert(end(v), begin(r), end(r));
        it = begin(v) + pos;
      }
    }
  }

  /// Splits the primitive \p t against the range of primitives \p r.
  template <typename T, typename Rng, typename SplitF>
  constexpr auto operator()(T t, Rng&& r, SplitF&& sf) const noexcept {
    static_assert(Range<Rng>{});
    static_assert(not Range<T>{});
    small_vector<T, 10> out{std::move(t)};
    ranges(out, r, std::forward<SplitF>(sf));
    return out;
  }
};

}  // namespace split_range_detail

namespace {
constexpr auto const& split_range
 = static_const<split_range_detail::split_range_fn>::value;
}

}  // namespace hm3::geometry
