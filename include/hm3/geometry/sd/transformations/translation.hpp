#pragma once
/// \file
///
/// Translation transformation: translates a signed-distance field to a point.
#include <hm3/geometry/sd/core.hpp>
#include <hm3/math/translation.hpp>

namespace hm3::geometry::sd {

namespace translation_detail {

struct translation_op_fn {
  /// Translates the signed-distance field to be evaluated on the point \p p by
  /// \p v.
  template <typename P, typename V, dim_t Ad>
  constexpr auto operator()(P&& p, V&& v, dim_vt<Ad>) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    math::inverse_translation(std::forward<V>(v), dim_v<Ad>)
    * std::forward<P>(p));
};

}  // namespace translation_detail

namespace {
static constexpr auto const& translation_op
 = static_const<translation_detail::translation_op_fn>::value;
}

/// Adapts a Signed-Distance function with a translation.
template <typename Sd>
struct translation : Sd {
  point<ad_v<Sd>> point_ = point<ad_v<Sd>>::constant(0.);

  template <typename P, typename... Args>
  translation(P xc, Args&&... args)
   : Sd(std::forward<Args>(args)...), point_(std::move(xc())) {
    static_assert(Point<P, ad_v<Sd>>());
  }

  template <typename P>
  void set_centroid(P p) noexcept {
    point_ = unwrap(p);
  }

  template <typename P>
  constexpr auto operator()(P const& x) const noexcept {
    return static_cast<Sd const&>(*this)(
     translation_op(unwrap(x), unwrap(point_), ad_v<Sd>));
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace hm3::geometry::sd
