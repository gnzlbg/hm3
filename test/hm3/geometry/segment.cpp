#include <hm3/geometry/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd> void basic_segment_test() {
  using namespace hm3;
  using namespace geometry;

  static constexpr dim_t nd = Nd;
  using l_t                 = segment<nd>;
  using p_t                 = point<nd>;

  static_assert(Dimensional<l_t>{}, "");
  static_assert(!Polygon<l_t>{}, "");

  p_t zero = p_t::constant(0.);
  p_t one  = p_t::constant(1.);
  p_t mone = p_t::constant(-1.);

  // constructors:
  auto l0 = l_t::through(zero, one);
  auto l1 = l_t::at(zero, one);

  auto l2 = l_t::through(one, zero);
  auto l3 = l_t::at(one, mone);

  auto d0 = p_t::constant(1. / std::sqrt(nd));
  auto d1 = p_t::constant(-1. / std::sqrt(nd));
  {
    l_t l4;
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

  // direction:
  CHECK(direction(l0) == d0);
  CHECK(direction(l1) == d0);

  CHECK(direction(l2) == d1);
  CHECK(direction(l3) == d1);

  // length:
  CHECK(length(l0) == length(l1));
  CHECK(length(l0) == length(l2));
  CHECK(length(l0) == length(l3));

  // centroid:
  CHECK(centroid(l0) == p_t::constant(0.5));
  CHECK(centroid(l0) == centroid(l1));
  CHECK(centroid(l0) == centroid(l2));
  CHECK(centroid(l0) == centroid(l3));

  // bounding box:
  auto bb = geometry::box<nd>{zero, one};
  CHECK(square_bounding_box(l0) == bb);
  CHECK(square_bounding_box(l0) == square_bounding_box(l1));
  CHECK(square_bounding_box(l0) == square_bounding_box(l2));
  CHECK(square_bounding_box(l0) == square_bounding_box(l3));

  auto abb = aabb<nd>{zero, one};
  CHECK(axis_aligned_bounding_box(l0) == abb);
  CHECK(axis_aligned_bounding_box(l0) == axis_aligned_bounding_box(l1));
  CHECK(axis_aligned_bounding_box(l0) == axis_aligned_bounding_box(l2));
  CHECK(axis_aligned_bounding_box(l0) == axis_aligned_bounding_box(l3));

  // flip
  CHECK(l0 != l2);
  CHECK(flip_direction(l0) == l2);
  CHECK(l1 != l3);
  CHECK(l1 == flip_direction(l3));
}

int main() {
  basic_segment_test<1>();
  basic_segment_test<2>();
  basic_segment_test<3>();

  {  // 2D
    using namespace hm3;
    using namespace geometry;

    static constexpr dim_t nd = 2;
    using l_t                 = segment<nd>;
    using p_t                 = point<nd>;

    p_t zero = p_t::constant(0.);
    p_t one  = p_t::constant(1.);

    auto l0 = l_t::through(zero, one);
    auto l1 = l_t::through(one, zero);

    auto d0 = p_t::constant(1. / std::sqrt(2));
    auto d1 = p_t::constant(-1. / std::sqrt(2));

    auto n0 = d0;
    n0(0) *= -1.;
    auto n1 = d0;
    n1(1) *= -1.;

    CHECK(direction(l0) == d0);
    CHECK(direction(l1) == d1);

    CHECK(normal(l0) == n0);
    CHECK(normal(l1) == n1);

    auto l2  = l_t::through(zero, p_t{{1.0, 0.5}});
    auto abb = aabb<nd>(zero, p_t{{1.0, 0.5}});
    auto bb  = geometry::box<nd>(p_t{{0.5, 0.25}}, 1.0);
    CHECK(axis_aligned_bounding_box(l2) == abb);
    CHECK(square_bounding_box(l2) == bb);
  }
  return test::result();
}
