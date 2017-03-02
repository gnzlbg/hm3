/// \file
///
/// Segment tests.
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::segment_primitive::segment<1>;
template struct hm3::geometry::segment_primitive::segment<2>;
template struct hm3::geometry::segment_primitive::segment<3>;

template <dim_t Ad>
void basic_segment_test() {
  using namespace hm3;
  using namespace geometry;

  static constexpr dim_t ad = Ad;
  using s_t                 = segment<ad>;
  using p_t                 = point<ad>;
  using v_t                 = vec<ad>;

  static_assert(AmbientDimension<s_t>{}, "");
  static_assert(AmbientDimension<s_t, Ad>{}, "");

  static_assert(ElementDimension<s_t>{}, "");
  static_assert(ElementDimension<s_t, Ad>{}, "");
  static_assert(ElementDimension<s_t, Ad, 1>{}, "");

  static_assert(GeometryObject<s_t>{}, "");
  static_assert(GeometryObject<s_t, Ad>{}, "");
  static_assert(GeometryObject<s_t, Ad, 1>{}, "");

  static_assert(!Line<s_t>{}, "");
  static_assert(!Line<s_t, Ad>{}, "");

  static_assert(Segment<s_t>{}, "");
  static_assert(Segment<s_t, Ad>{}, "");

  p_t zero = p_t::constant(0.);
  p_t one  = p_t::constant(1.);
  p_t mone = p_t::constant(-1.);

  // constructors:
  auto l0 = s_t(zero, one);
  auto l1 = s_t(zero, as_v(one));

  auto l2 = s_t(one, zero);
  auto l3 = s_t(one, as_v(mone));

  auto d0 = p_t::constant(1. / std::sqrt(ad));
  auto d1 = p_t::constant(-1. / std::sqrt(ad));
  {
    s_t l4;
    l4 = l0;
    CHECK(l4 == l0);
  }

  // reltional:
  CHECK(l0 == l1);
  CHECK(l2 == l3);
  CHECK(!(l0 != l1));
  CHECK(!(l2 != l3));
  CHECK(l0 != l2);
  CHECK(l1 != l3);

  {  // approx
    CHECK(approx(l0, l1));
    CHECK(approx(l2, l3));
    CHECK(not approx(l0, l2));
    CHECK(not approx(l1, l3));
  }

  // direction:
  CHECK(direction(l0) == as_v(d0));
  CHECK(direction(l1) == as_v(d0));

  CHECK(direction(l2) == as_v(d1));
  CHECK(direction(l3) == as_v(d1));
  // length:
  CHECK(length(l0) == length(l1));
  CHECK(length(l0) == length(l2));
  CHECK(length(l0) == length(l3));

  // centroid:
  CHECK(centroid(l0) == p_t::constant(0.5));
  CHECK(centroid(l0) == centroid(l1));
  CHECK(centroid(l0) == centroid(l2));
  CHECK(centroid(l0) == centroid(l3));

  // bounding length:
  CHECK(bounding_length(l0, 0) == 1.0);
  CHECK(bounding_length.max(l0) == 1.0);
  CHECK(bounding_length.all(l0) == v_t{v_t::ones()});

  // bounding box:
  auto bb = geometry::box<ad>{zero, one};
  CHECK(bounding_volume.box(l0) == bb);
  CHECK(bounding_volume.box(l0) == bounding_volume.box(l1));
  CHECK(bounding_volume.box(l0) == bounding_volume.box(l2));
  CHECK(bounding_volume.box(l0) == bounding_volume.box(l3));

  auto abb = aabb<ad>{zero, one};
  CHECK(bounding_volume.aabb(l0) == abb);
  CHECK(bounding_volume.aabb(l0) == bounding_volume.aabb(l1));
  CHECK(bounding_volume.aabb(l0) == bounding_volume.aabb(l2));
  CHECK(bounding_volume.aabb(l0) == bounding_volume.aabb(l3));

  // flip
  CHECK(l0 != l2);
  CHECK(direction.invert(l0) == l2);
  CHECK(l1 != l3);
  CHECK(l1 == direction.invert(l3));

  {  // check vertices
    CHECK(vertex(l0, 0) == l0.x(0));
    CHECK(vertex(l0, 1) == l0.x(1));
    CHECK(vertex_size(l0) == 2_u);
    CHECK(ranges::size(vertices(l0)) == 2_u);
    CHECK(vertices(l0)[0] == l0.x(0));
    CHECK(vertices(l0)[1] == l0.x(1));
  }

  {  // check edges
    CHECK(edge(l0, 0) == l0);
    CHECK(ranges::size(edges(l0)) == 1_u);
    CHECK(edges(l0)[0] == l0);
  }
}

int main() {
  basic_segment_test<1>();
  basic_segment_test<2>();
  basic_segment_test<3>();

  {  // 1D
    using namespace hm3;
    using namespace geometry;

    static constexpr dim_t ad = 1;
    using s_t                 = segment<ad>;
    using p_t                 = point<ad>;

    p_t zero = p_t::constant(0.);
    p_t one  = p_t::constant(1.);
    auto l1  = s_t(one, zero);
    CHECK(volume(l1) == length(l1));

    {  // point parameter
      auto s = s_t(p_t(0.5), p_t(1.0));
      CHECK(line_intersection_parameter(s, p_t(0.5), on_segment).value() == 0.);

      CHECK(line_intersection_parameter(s, p_t(1.0), on_segment).value() == 1.);
      CHECK(line_intersection_parameter(s, p_t(0.5), on_segment_line).value()
            == 0.);
      CHECK(line_intersection_parameter(s, p_t(1.0), on_segment_line).value()
            == 1.);

      CHECK(line_intersection_parameter(s, p_t(0.0), on_segment_line).value()
            == -1.);
      CHECK(line_intersection_parameter(s, p_t(1.5), on_segment_line).value()
            == 2.);
      CHECK(not line_intersection_parameter(s, p_t(0.0), on_segment));
      CHECK(not line_intersection_parameter(s, p_t(1.5), on_segment));
    }
  }

  {  // 2D
    using namespace hm3;
    using namespace geometry;

    static constexpr dim_t ad = 2;
    using s_t                 = segment<ad>;
    using p_t                 = point<ad>;
    using v_t                 = vec<ad>;

    p_t zero = p_t::constant(0.);
    p_t one  = p_t::constant(1.);

    auto s0 = s_t(zero, one);
    auto s1 = s_t(one, zero);

    auto d0 = v_t::constant(1. / std::sqrt(2));
    auto d1 = v_t::constant(-1. / std::sqrt(2));

    auto n0 = d0;
    n0(0) *= -1.;
    auto n1 = d0;
    n1(1) *= -1.;

    CHECK(direction(s0) == d0);
    CHECK(direction(s1) == d1);

    CHECK(geometry::approx(normal(s0), n0));
    CHECK(geometry::approx(normal(s1), n1));
    CHECK(area(s1) == length(s1));

    auto l2  = s_t(zero, p_t{{1.0, 0.5}});
    auto abb = aabb<ad>(zero, p_t{{1.0, 0.5}});
    auto bb  = geometry::box<ad>(p_t{{0.5, 0.25}}, 1.0);
    CHECK(bounding_volume.aabb(l2) == abb);
    CHECK(bounding_volume.box(l2) == bb);

    using geometry::distance;

    p_t p0{-1., -1.};
    CHECK(math::approx(distance(s0, p0), std::sqrt(2.)));
    p_t p1{2., 2.};
    CHECK(math::approx(distance(s0, p1), std::sqrt(2.)));

    p_t p2{0., .5};
    CHECK(distance(s0, p2) == std::sqrt(2. * std::pow(0.25, 2.)));

    p_t p3{.5, .0};
    CHECK(distance(s0, p3) == std::sqrt(2. * std::pow(0.25, 2.)));

    p_t p4{-1, .0};
    CHECK(distance(s0, p4) == 1.);

    p_t p5{2., 1.};
    CHECK(distance(s0, p5) == 1.);

    p_t p6{0., -1.};
    CHECK(distance(s0, p6) == 1.);

    p_t p7{1., 2.};
    CHECK(distance(s0, p7) == 1.);
  }

  // TODO: test relative position

  return test::result();
}
