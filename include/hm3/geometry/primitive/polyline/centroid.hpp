#include <hm3/geometry/primitive/polyline/faces.hpp>
#include <hm3/geometry/primitive/segment/centroid.hpp>
#include <hm3/geometry/primitive/segment/integral.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, CONCEPT_REQUIRES_(Polyline<P>{})>
constexpr auto centroid(P&& p) noexcept -> point<dimension_v<P>> {
  constexpr dim_t nd = dimension_v<P>;
  using p_t          = point<nd>;

  HM3_ASSERT(face_size(p) > 0,
             "cannot compute the centroid of an empty polyline");

  auto x_c         = p_t::constant(0.);
  num_t length_sum = 0.;

  for (auto&& s : faces(p)) {
    auto segment_length   = integral(s, path_integral<segment<nd>>{});
    auto segment_centroid = centroid(s);
    x_c() += segment_length * segment_centroid();
    length_sum += segment_length;
  }

  HM3_ASSERT(length_sum > 0., "sum of the polyline lengths is {} !> 0.",
             length_sum);
  x_c() /= length_sum;
  return x_c;
}

}  // namespace hm3::geometry::polyline_primitive
