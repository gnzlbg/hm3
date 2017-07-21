/// \file
///
/// Geometry point tests.
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/io/ascii.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::point_primitive::point<1>;
template struct hm3::geometry::point_primitive::point<2>;
template struct hm3::geometry::point_primitive::point<3>;

template <typename T, dim_t Ad>
void check_point_concepts() {
  // note: all concepts checked independently here,
  // useful when something breaks:

  static_assert(AmbientDimension<T>{}, "");
  static_assert(AmbientDimension<T, Ad>{}, "");

  static_assert(ElementDimension<T>{}, "");
  static_assert(ElementDimension<T, Ad>{}, "");
  static_assert(ElementDimension<T, Ad, 0>{}, "");

  static_assert(GeometryObject<T>{}, "");
  static_assert(GeometryObject<T, Ad>{}, "");
  static_assert(GeometryObject<T, Ad, 0>{}, "");

  static_assert(Vertex<T>{}, "");
  static_assert(Vertex<T, Ad>{}, "");

  static_assert(VertexAccess<T>{}, "");

  static_assert(Point<T>{}, "");
  static_assert(Point<T, Ad>{}, "");
}

int main() {
  using namespace geometry;

  using p1t = point<1>;
  using p2t = point<2>;
  using p3t = point<3>;

  {  // check constructors, dimension, and concepts:

    constexpr auto p1 = p1t{0.};
    constexpr auto p2 = p2t{0., 0.};
    constexpr auto p3 = p3t{0., 0., 0.};

    STATIC_CHECK(ambient_dimension(p1) == 1_su);
    STATIC_CHECK(ambient_dimension(p2) == 2_su);
    STATIC_CHECK(ambient_dimension(p3) == 3_su);

    STATIC_CHECK(element_dimension(p1) == 0_su);
    STATIC_CHECK(element_dimension(p2) == 0_su);
    STATIC_CHECK(element_dimension(p3) == 0_su);

    check_point_concepts<p1t, 1>();
    check_point_concepts<p2t, 2>();
    check_point_concepts<p3t, 3>();
  }

  {  // check values:
    constexpr auto p1 = p1t{0.};
    constexpr auto p3 = p3t{0., 1., 2.};
    const p3t p4      = p3t::constant(1.);

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);
    CHECK(p4(0) == 1.);
    CHECK(p4(1) == 1.);
    CHECK(p4(2) == 1.);
  }
  {  // check relational operators:
    constexpr auto p1a = p1t{0.};
    constexpr auto p1b = p1t{1.};
    constexpr auto p3a = p3t{0., 1., 2.};
    constexpr auto p3b = p3t{2., 0., 1.};
    constexpr auto p3c = p3t{0., 0., 0.};
    constexpr auto p3d = p3t{1., 0., 0.};

    CHECK(p1a == p1a);
    CHECK(p1a != p1b);
    CHECK(p3a == p3a);
    CHECK(p3a != p3b);
    CHECK(!(p3c == p3d));
    CHECK(p3c != p3d);
  }

  {  // check constexpr:
    constexpr auto p1 = p1t{0.};
    constexpr auto p2 = p2t{0., 1.};
    constexpr auto p3 = p3t{0., 1., 2.};

    STATIC_CHECK(ambient_dimension(p1) == 1_su);
    STATIC_CHECK(ambient_dimension(p2) == 2_su);
    STATIC_CHECK(ambient_dimension(p3) == 3_su);

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p3(0) == 0.);
    STATIC_CHECK(p3(1) == 1.);
    STATIC_CHECK(p3(2) == 2.);
  }

  {  // check assignment:
    constexpr auto p1 = p2t{0., 1.};
    constexpr auto p2 = p1;

    STATIC_CHECK(ambient_dimension(p1) == ambient_dimension(p2));

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p1(1) == 1.);
    STATIC_CHECK(p2(0) == 0.);
    STATIC_CHECK(p2(1) == 1.);

    constexpr auto p3 = p1t{1.5};
    constexpr auto p4 = p3;

    STATIC_CHECK(ambient_dimension(p3) == ambient_dimension(p4));

    STATIC_CHECK(p3(0) == 1.5);
    STATIC_CHECK(p4(0) == 1.5);
  }

  {  // check vertices
    constexpr auto p1 = p1t{1.5};
    constexpr auto p2 = p2t{0., 1.};

    CHECK(vertex.size(p1) == 1_su);
    CHECK(vertex.size(p2) == 1_su);

    CHECK(vertex(p1, 0) == p1);
    CHECK(vertex.first(p1) == p1);
    CHECK(vertex.last(p1) == p1);
    CHECK(vertex(p2, 0) == p2);

    for (auto&& v : vertices(p1)) { CHECK(v == p1); }
    for (auto&& v : vertices(p2)) { CHECK(v == p2); }

    for (auto&& i : vertex.indices(p1)) { CHECK(vertex(p1, i) == p1); }
    for (auto&& i : vertex.indices(p2)) { CHECK(vertex(p2, i) == p2); }
  }

  {  // check centroid
    constexpr auto p1 = p1t{1.5};
    constexpr auto p2 = p2t{0., 1.};
    CHECK(centroid(p2) == p2);
    CHECK(centroid(p1) == p1);
  }

  {  // check 1D point
    constexpr auto p1 = p1t{1.5};
    CHECK(integral.surface(p1) == 1.);
    CHECK(area(p1) == 1.);
    CHECK(normal(p1) == vec<1>::constant(1.));
  }

  { // distance between two points
   {// 1D

    constexpr auto p1 = p1t{-1.0};
  constexpr auto p2 = p1t{1.0};
  constexpr auto p3 = p1t{0.0};

  CHECK(distance.centroid(p1, p2) == 2.);
  CHECK(distance.centroid(p2, p1) == 2.);
  CHECK(distance.centroid(p1, p3) == 1.);
  CHECK(distance.centroid(p3, p1) == 1.);
  CHECK(distance.centroid(p2, p3) == 1.);
  CHECK(distance.centroid(p3, p2) == 1.);
}

{  // 2D
  constexpr auto p1 = p2t{-1.0, -1.0};
  constexpr auto p2 = p2t{1.0, 1.0};
  constexpr auto p3 = p2t{0.0, 0.0};

  CHECK(distance.centroid(p1, p2) == std::sqrt(8.));
  CHECK(distance.centroid(p2, p1) == std::sqrt(8.));
  CHECK(distance.centroid(p1, p3) == std::sqrt(2.));
  CHECK(distance.centroid(p3, p1) == std::sqrt(2.));
  CHECK(distance.centroid(p2, p3) == std::sqrt(2.));
  CHECK(distance.centroid(p3, p2) == std::sqrt(2.));
}
{  // 3D
  constexpr auto p1 = p3t{1.0, 1.0, 1.0};
  constexpr auto p2 = p3t{0.0, 0.0, 0.0};
  CHECK(distance.centroid(p1, p2) == std::sqrt(3.));
  CHECK(distance.centroid(p2, p1) == std::sqrt(3.));
}
}

{  // single element bug
  using p_t                      = p1t;
  optional<pair<num_t, num_t>> o = make_pair(3.5, 2.5);
  p_t p{first(o.value())};
  CHECK(p(0) == 3.5);
  p_t p2{second(o.value())};
  CHECK(p2(0) == 2.5);

  p_t p3{first(o.value())};
  CHECK(p3(0) == p(0));
  p_t p4{second(o.value())};
  CHECK(p4(0) == p2(0));
}

{ // exhaustive constructor check
 {// 1D
  using p_t = p1t;
p_t p1(3.5);
p_t p2{3.5};
auto p3 = p_t(3.5);
auto p4 = p_t{3.5};

CHECK(p1(0) == 3.5);
CHECK(p2(0) == 3.5);
CHECK(p3(0) == 3.5);
CHECK(p4(0) == 3.5);
}
{  // 2D
  using p_t = p2t;
  p_t p1(3.5, 4.5);
  p_t p2{3.5, 4.5};
  auto p3 = p_t(3.5, 4.5);
  auto p4 = p_t{3.5, 4.5};

  CHECK(p1(0) == 3.5);
  CHECK(p2(0) == 3.5);
  CHECK(p3(0) == 3.5);
  CHECK(p4(0) == 3.5);
  CHECK(p1(1) == 4.5);
  CHECK(p2(1) == 4.5);
  CHECK(p3(1) == 4.5);
  CHECK(p4(1) == 4.5);
}
{  // 2D
  using p_t = p3t;
  p_t p1(3.5, 4.5, 5.5);
  p_t p2{3.5, 4.5, 5.5};
  auto p3 = p_t(3.5, 4.5, 5.5);
  auto p4 = p_t{3.5, 4.5, 5.5};

  CHECK(p1(0) == 3.5);
  CHECK(p2(0) == 3.5);
  CHECK(p3(0) == 3.5);
  CHECK(p4(0) == 3.5);
  CHECK(p1(1) == 4.5);
  CHECK(p2(1) == 4.5);
  CHECK(p3(1) == 4.5);
  CHECK(p4(1) == 4.5);
  CHECK(p1(2) == 5.5);
  CHECK(p2(2) == 5.5);
  CHECK(p3(2) == 5.5);
  CHECK(p4(2) == 5.5);
}
}

{  // construction conversion to associated vector type
  p2t p(3.5, 4.5);
  vec<2> v(3.5, 4.5);

  CHECK(p2t(v) == p);
  CHECK(as_v(p) == v);
}

return test::result();
}
