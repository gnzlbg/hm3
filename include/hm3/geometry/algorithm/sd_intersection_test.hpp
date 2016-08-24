#pragma once
/// \file
///
/// Does a primitive intersect the zero-th level of a signed-distance field?
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/sd/concepts.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

/// Result of an intersection test of a primitive with a signed-distance
/// function:
///
/// \note This representation is linked with `sd_interesction_test_t` (don't
/// change one without changing the other).
enum class sd_interesction_test_t : suint_t { positive, negative, intersected };

namespace sd_intersection_test_detail {

/// Is the range of signums of signed-distance values \p rng at the vertices of
/// a primitive intersected by the zero-th level of a signed-distance function?
template <
 typename SDValueRng,
 CONCEPT_REQUIRES_(Range<SDValueRng>{}
                   and Same<sint_t, uncvref_t<range_value_t<SDValueRng>>>{})>
constexpr sd_intersection_test_t sd_intersection_test(
 SDValueRng&& rng) noexcept {
  using sdr = sd_intersection_test_t;
  auto it   = begin(rng);
  auto e    = end(rng);
  HM3_ASSERT(it != e, "rng cannot be empty");

  sint_t v = *it;
  if (v == 0) { return sdr::intersected; }
  while (++it != e) {
    if (v != *it) { return sdr::intersected; }
  }
  return v > 0 ? sdr::positive : sdr::negative;
}

/// Is the range of signed-distance values \p rng at the vertices of a
/// primitive intersected by the zero-th level of a signed-distance function?
template <
 typename SDValueRng,
 CONCEPT_REQUIRES_(Range<SDValueRng>{}
                   and Same<num_t, uncvref_t<range_value_t<SDValueRng>>>{})>
constexpr sd_intersection_test_t sd_intersection_test(
 SDValueRng&& rng) noexcept {
  return sd_signum_intersection_test(
   rng | view::transform([](auto&& v) { return math::signum(v); }));
}

/// Is the primitive \p p intersected by the signed-distance field \p sdf?
template <typename P, typename SD, dim_t Nd = dimension_v<P>,
          CONCEPT_REQUIRES_(Primitive<P>{} and SignedDistance<SD, ND>{})>
static constexpr sd_intersection_test_t sd_intersection_test(
 P&& p, SD&& sdf) noexcept {
  return sd_intersection_test(vertices(p)
                              | transform([&sdf](auto&& x) { return sdf(x); }));
}

struct sd_intersection_test_generic_fn {
  template <typename T, typename U>
  static constexpr auto impl_select(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    sd_intersection_test(std::forward<U>(u), std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto impl_select(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    sd_intersection_test(std::forward<T>(t), std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl_select(std::forward<T>(t),
                                                    std::forward<U>(u), 0));

  template <typename Rng>
  constexpr auto operator()(Rng&& rng) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    sd_intersection_test(std::forward<Rng>(rng)));
};

}  // namespace sd_intersection_test_detail

namespace {
static constexpr auto const& sd_intersection_test
 = static_const<sd_intersection_test_detail::sd_intersection_test_fn>::value;
}  // namespace

}  // namespace hm3::geometry
