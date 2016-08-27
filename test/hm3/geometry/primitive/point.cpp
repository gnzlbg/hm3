/// \file
///
/// Geometry point tests
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::point_primitive::point<1>;
template struct hm3::geometry::point_primitive::point<2>;
template struct hm3::geometry::point_primitive::point<3>;

static_assert(Dimensional<point<1>>{}, "");
static_assert(Dimensional<point<2>>{}, "");
static_assert(Dimensional<point<3>>{}, "");

static_assert(Dimensional<point<1>, 1>{}, "");
static_assert(Dimensional<point<2>, 2>{}, "");
static_assert(Dimensional<point<3>, 3>{}, "");

static_assert(Primitive<point<1>>{}, "");
static_assert(Primitive<point<2>>{}, "");
static_assert(Primitive<point<3>>{}, "");

static_assert(Surface<point<1>>{}, "");
static_assert(!Surface<point<2>>{}, "");
static_assert(!Surface<point<3>>{}, "");

static_assert(!Segment<point<1>>{}, "");
static_assert(!Segment<point<2>>{}, "");
static_assert(!Segment<point<3>>{}, "");

static_assert(!Volume<point<1>>{}, "");
static_assert(!Volume<point<2>>{}, "");
static_assert(!Volume<point<3>>{}, "");

int main() {
  using namespace geometry;

  {  // check constructors and dimension:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p2 = point<2>{0., 0.};
    constexpr auto p3 = point<3>{0., 0., 0.};

    STATIC_CHECK(dimension(p1) == 1_su);
    STATIC_CHECK(dimension(p2) == 2_su);
    STATIC_CHECK(dimension(p3) == 3_su);
  }

  {  // check values:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p3 = point<3>{0., 1., 2.};
    const point<3> p4 = point<3>::constant(1.);

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);
    CHECK(p4(0) == 1.);
    CHECK(p4(1) == 1.);
    CHECK(p4(2) == 1.);
  }

  /* {  // check vector length:
    constexpr auto p1a = point<1>{0.};
    constexpr auto p1b = point<1>{1.};
    constexpr auto p1c = point<1>{2.};
    const point<1> p1d = point<1>::constant(1.);

    CHECK(length(p1a) == 0.);
    CHECK(length(p1b) == 1.);
    CHECK(length(p1c) == 2.);
    CHECK(length(p1d) == 1.);

    constexpr auto p2a = point<2>{0., 0.};
    constexpr auto p2b = point<2>{1., 1.};

    CHECK(length(p2a) == 0.);
    CHECK(length(p2b) == std::sqrt(2));

    constexpr auto p3a = point<3>{0., 0., 0.};
    constexpr auto p3b = point<3>{1., 1., 1.};

    CHECK(length(p3a) == 0.);
    CHECK(length(p3b) == std::sqrt(3));
    }*/

  {  // check relational operators:
    constexpr auto p1a = point<1>{0.};
    constexpr auto p1b = point<1>{1.};
    constexpr auto p3a = point<3>{0., 1., 2.};
    constexpr auto p3b = point<3>{2., 0., 1.};
    constexpr auto p3c = point<3>{0., 0., 0.};
    constexpr auto p3d = point<3>{1., 0., 0.};

    CHECK((p1a == p1a));
    CHECK((p1a != p1b));
    CHECK((p3a == p3a));
    CHECK((p3a != p3b));
    CHECK(!(p3c == p3d));
    CHECK((p3c != p3d));
  }

  {  // check constexpr:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p2 = point<2>{0., 1.};
    constexpr auto p3 = point<3>{0., 1., 2.};

    CHECK(dimension(p1) == 1_su);
    CHECK(dimension(p2) == 2_su);
    CHECK(dimension(p3) == 3_su);

    STATIC_CHECK(dimension(p1) == 1_su);
    STATIC_CHECK(dimension(p2) == 2_su);
    STATIC_CHECK(dimension(p3) == 3_su);

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p3(0) == 0.);
    STATIC_CHECK(p3(1) == 1.);
    STATIC_CHECK(p3(2) == 2.);
  }

  {  // check assignment:
    constexpr auto p1 = point<2>{0., 1.};
    constexpr auto p2 = p1;

    STATIC_CHECK(dimension(p1) == dimension(p2));

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p1(1) == 1.);
    STATIC_CHECK(p2(0) == 0.);
    STATIC_CHECK(p2(1) == 1.);

    constexpr auto p3 = point<1>{1.5};
    constexpr auto p4 = p3;

    STATIC_CHECK(dimension(p3) == dimension(p4));

    STATIC_CHECK(p3(0) == 1.5);
    STATIC_CHECK(p4(0) == 1.5);
  }

  {  // check vertices
    constexpr auto p1 = point<1>{1.5};
    constexpr auto p2 = point<2>{0., 1.};

    CHECK(vertex_size(p1) == 1_su);
    CHECK(vertex_size(p2) == 1_su);

    CHECK(vertex(p1, 0) == p1);
    CHECK(vertex(p2, 0) == p2);

    for (auto&& v : vertices(p1)) { CHECK(v == p1); }
    for (auto&& v : vertices(p2)) { CHECK(v == p2); }

    for (auto&& i : vertex_indices(p1)) { CHECK(vertex(p1, i) == p1); }
    for (auto&& i : vertex_indices(p2)) { CHECK(vertex(p2, i) == p2); }
  }

  {  // check centroid
    constexpr auto p1 = point<1>{1.5};
    constexpr auto p2 = point<2>{0., 1.};
    CHECK(centroid(p2) == p2);
    CHECK(centroid(p1) == p1);
  }

  {  // check 1D point
    constexpr auto p1 = point<1>{1.5};
    CHECK(integral.area(p1) == 1.);
    CHECK(area(p1) == 1.);
    CHECK(normal(p1) == vec<1>::constant(1.));
  }

  { // distance between two points
   {// 1D

    constexpr auto p1 = point<1>{-1.0};
  constexpr auto p2   = point<1>{1.0};
  constexpr auto p3   = point<1>{0.0};

  CHECK(distance.centroid(p1, p2) == 2.);
  CHECK(distance.centroid(p2, p1) == 2.);
  CHECK(distance.centroid(p1, p3) == 1.);
  CHECK(distance.centroid(p3, p1) == 1.);
  CHECK(distance.centroid(p2, p3) == 1.);
  CHECK(distance.centroid(p3, p2) == 1.);
}

{  // 2D
  constexpr auto p1 = point<2>{-1.0, -1.0};
  constexpr auto p2 = point<2>{1.0, 1.0};
  constexpr auto p3 = point<2>{0.0, 0.0};

  CHECK(distance.centroid(p1, p2) == std::sqrt(8.));
  CHECK(distance.centroid(p2, p1) == std::sqrt(8.));
  CHECK(distance.centroid(p1, p3) == std::sqrt(2.));
  CHECK(distance.centroid(p3, p1) == std::sqrt(2.));
  CHECK(distance.centroid(p2, p3) == std::sqrt(2.));
  CHECK(distance.centroid(p3, p2) == std::sqrt(2.));
}
{  // 3D
  constexpr auto p1 = point<3>{1.0, 1.0, 1.0};
  constexpr auto p2 = point<3>{0.0, 0.0, 0.0};
  CHECK(distance.centroid(p1, p2) == std::sqrt(3.));
  CHECK(distance.centroid(p2, p1) == std::sqrt(3.));
}
}

// {  // single element bug
//   using p_t = point<1>;
//   optional<pair<num_t, num_t>> o = make_pair(3.5, 2.5);
//   p_t p{o.value().first()};
//   CHECK(p(0) == 3.5);
//   p_t p2{o.value().second()};
//   CHECK(p2(0) == 2.5);

//   p_t p3{{o.value().first()}};
//   CHECK(p3(0) == p(0));
//   p_t p4{{o.value().second()}};
//   CHECK(p4(0) == p2(0));
// }

{    // exhaustive constructor check
  {  // 1D
    using p_t = point<1>;
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
    using p_t = point<2>;
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
    using p_t = point<3>;
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
return test::result();
}
