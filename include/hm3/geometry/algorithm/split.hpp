#pragma once
/// \file
///
/// Split one primitive with another
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace split_detail {

struct split_fn {
  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split(std::forward<U>(u),
                                              std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto split_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split(std::forward<T>(t),
                                              std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(split_impl(std::forward<T>(t),
                                                   std::forward<U>(u), 0));

  // Split each primitive in vector \p v against each primitive in range \p
  // rng (and the result is returned in the vector \p v)
  template <typename Vec, typename Rng>
  static auto range(Vec& v, Rng&& rng) {
    HM3_ASSERT(not v.empty(), "");
    for (auto&& p : rng) {
      for (auto&& it = begin(v); it != end(v);) {
        // Split the current polygon with the first polyline that works
        auto r = split_fn{}(*it, p);

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
};

}  // namespace split_detail

namespace {
static constexpr auto const& split
 = static_const<split_detail::split_fn>::value;
}  // namespace

}  // namespace hm3::geometry
