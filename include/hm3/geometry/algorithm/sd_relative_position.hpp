#pragma once
/// \file
///
/// Relative position of a primitive w.r.t. a signed-distance field.
#include <hm3/geometry/algorithm/sd_sign.hpp>
#include <hm3/geometry/concept/signed_distance.hpp>
#include <hm3/geometry/relative_position.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry::sd {

namespace relative_position_detail {

struct relative_position_fn {
  /// Relative position of the signed-distance \p value.
  constexpr relative_position_t operator()(num_t value, num_t abs_tol,
                                           num_t rel_tol) const noexcept {
    switch (sign(value, abs_tol, rel_tol)) {
      case math::signum_t::type::zero: {
        return intersected;
      }
      case math::signum_t::type::positive: {
        return inside;
      }
      case math::signum_t::type::negative: {
        return outside;
      }
    }
    HM3_FATAL_ERROR("unreachable");
  }

  /// Relative position of a primitive from the range of signs at its vertices.
  template <
   typename Rng,
   CONCEPT_REQUIRES_(Range<Rng>{}
                     and Same<math::signum_t, uncvref_t<range_value_t<Rng>>>{})>
  constexpr relative_position_t operator()(Rng&& rng) const noexcept {
    auto it      = begin(rng);
    const auto e = end(rng);
    HM3_ASSERT(it != e, "rng cannot be empty");

    const math::signum_t v = *it;
    if (v == math::signum_t::zero()) { return intersected; }
    while (++it != e) {
      if (v != *it) { return intersected; }
    }
    return v == math::signum_t::positive() ? inside : outside;
  }

  /// Relative position of a primitive from the range of signed-distance value
  /// at its vertices.
  template <typename Rng,
            CONCEPT_REQUIRES_(Range<Rng>{}
                              and Same<num_t, uncvref_t<range_value_t<Rng>>>{})>
  constexpr relative_position_t operator()(Rng&& rng, num_t abs_tol,
                                           num_t rel_tol) const noexcept {
    return relative_position_fn{}(
     rng | view::transform([abs_tol, rel_tol](auto&& v) {
       return sign(v, abs_tol, rel_tol);
     }));
  }
};

}  // namespace relative_position_detail

namespace {
constexpr auto const& relative_position = static_const<
 with_default_tolerance<relative_position_detail::relative_position_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
