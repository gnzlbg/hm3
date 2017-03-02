#ifdef ABC
#pragma once
/// \file
///
/// Mesh of simplices
#include <hm3/geometry/algorithm/merge.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/data_structure/bvh.hpp>
#include <hm3/geometry/data_structure/simplex_array.hpp>

namespace hm3::geometry {

template <typename P>
struct poly_surface {};

template <>
struct poly_surface<point<1>> {
  using type = point<1>;
};

template <>
struct poly_surface<segment<2>> {
  using type = small_polyline<2, 3>;
};

// template <>
// struct poly_surface<triangle<3>> {
//   using type = small_polysurface<3, 4>;
// };

template <typename T>
using poly_surface_t = typename poly_surface<T>::type;

/// Mesh of simplices
template <dim_t Nd>
struct mesh : simplex_array<Nd>, bounding_volume_hierarchy<Nd> {
  using array_t               = simplex_array<Nd>;
  using bvh_t                 = bounding_volume_hierarchy<Nd>;
  using simplex_idx           = bvh::simplex_idx;
  using simplex_t             = typename array_t::simplex_t;
  using intersection_result_t = small_vector<simplex_idx, 3>;

  template <typename SimplexRange>
  mesh(SimplexRange const& simplices) : array_t(simplices) {
    this->rebuild(static_cast<array_t const&>(*this));
  }

  bool intersection_test(aabb<Nd> const& b) const {
    return this->aabb_intersection_test(b);
  }

  intersection_result_t intersection(aabb<Nd> const& b) const {
    return this->aabb_intersection(b);
  }

  simplex_t simplex(simplex_idx i) const {
    HM3_ASSERT(i, "invalid simplex index");
    return this->face(*i);
  }

  auto split(aabb<Nd> const& b) const {
    vector<small_polygon<Nd, 10>> result;  // TODO small_

    if (not intersection_test(b)) { return result; }

    auto simplices
     = intersection(b) | view::transform([&](auto&& s) { return simplex(s); });

    vector<poly_surface_t<simplex_t>> poly_surfaces;  // TODO small_

    geometry::merge.range(simplices, poly_surfaces);

    result.reserve(10);  // TODO: should probably be ~3 or so
    result.push_back(b);
    geometry::split(result, poly_surfaces);
    return result;
  }
};

}  // namespace hm3::geometry
#endif
