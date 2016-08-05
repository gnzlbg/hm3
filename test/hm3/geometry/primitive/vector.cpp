/// \file
///
/// Geometry point tests
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::vec_primitive::vec<1>;
template struct hm3::geometry::vec_primitive::vec<2>;
template struct hm3::geometry::vec_primitive::vec<3>;

static_assert(Dimensional<vec<1>>{}, "");
static_assert(Dimensional<vec<2>>{}, "");
static_assert(Dimensional<vec<3>>{}, "");

static_assert(Dimensional<vec<1>, 1>{}, "");
static_assert(Dimensional<vec<2>, 2>{}, "");
static_assert(Dimensional<vec<3>, 3>{}, "");

static_assert(SemiRegular<vec<1>>{}, "");
static_assert(SemiRegular<vec<2>>{}, "");
static_assert(SemiRegular<vec<3>>{}, "");

static_assert(Regular<vec<1>>{}, "");
static_assert(Regular<vec<2>>{}, "");
static_assert(Regular<vec<3>>{}, "");

static_assert(!Primitive<vec<1>>{}, "");
static_assert(!Primitive<vec<2>>{}, "");
static_assert(!Primitive<vec<3>>{}, "");

static_assert(!Surface<vec<1>>{}, "");
static_assert(!Surface<vec<2>>{}, "");
static_assert(!Surface<vec<3>>{}, "");

static_assert(!Segment<vec<1>>{}, "");
static_assert(!Segment<vec<2>>{}, "");
static_assert(!Segment<vec<3>>{}, "");

static_assert(!Volume<vec<1>>{}, "");
static_assert(!Volume<vec<2>>{}, "");
static_assert(!Volume<vec<3>>{}, "");

int main() {
  using namespace geometry;

  {  // check constructors and dimension:
    constexpr auto p1 = vec<1>{0.};
    constexpr auto p2 = vec<2>{0., 0.};
    constexpr auto p3 = vec<3>{0., 0., 0.};

    STATIC_CHECK(dimension(p1) == 1_su);
    STATIC_CHECK(dimension(p2) == 2_su);
    STATIC_CHECK(dimension(p3) == 3_su);
  }

  {  // check values:
    constexpr auto p1 = vec<1>{0.};
    constexpr auto p3 = vec<3>{0., 1., 2.};
    const vec<3> p4   = vec<3>::constant(1.);

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);
    CHECK(p4(0) == 1.);
    CHECK(p4(1) == 1.);
    CHECK(p4(2) == 1.);
  }

  {  // check vector length:
    constexpr auto p1a = vec<1>{0.};
    constexpr auto p1b = vec<1>{1.};
    constexpr auto p1c = vec<1>{2.};
    const vec<1> p1d   = vec<1>::constant(1.);

    CHECK(length(p1a) == 0.);
    CHECK(length(p1b) == 1.);
    CHECK(length(p1c) == 2.);
    CHECK(length(p1d) == 1.);

    constexpr auto p2a = vec<2>{0., 0.};
    constexpr auto p2b = vec<2>{1., 1.};

    CHECK(length(p2a) == 0.);
    CHECK(length(p2b) == std::sqrt(2));

    constexpr auto p3a = vec<3>{0., 0., 0.};
    constexpr auto p3b = vec<3>{1., 1., 1.};

    CHECK(length(p3a) == 0.);
    CHECK(length(p3b) == std::sqrt(3));
  }

  {  // check relational operators:
    constexpr auto p1a = vec<1>{0.};
    constexpr auto p1b = vec<1>{1.};
    constexpr auto p3a = vec<3>{0., 1., 2.};
    constexpr auto p3b = vec<3>{2., 0., 1.};

    CHECK((p1a == p1a));
    CHECK((p1a != p1b));
    CHECK((p3a == p3a));
    CHECK((p3a != p3b));
  }

  {  // check constexpr:
    constexpr auto p1 = vec<1>{0.};
    constexpr auto p2 = vec<2>{0., 1.};
    constexpr auto p3 = vec<3>{0., 1., 2.};

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
    constexpr auto p1 = vec<2>{0., 1.};
    constexpr auto p2 = p1;

    STATIC_CHECK(dimension(p1) == dimension(p2));

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p1(1) == 1.);
    STATIC_CHECK(p2(0) == 0.);
    STATIC_CHECK(p2(1) == 1.);

    constexpr auto p3 = vec<1>{1.5};
    constexpr auto p4 = p3;

    STATIC_CHECK(dimension(p3) == dimension(p4));

    STATIC_CHECK(p3(0) == 1.5);
    STATIC_CHECK(p4(0) == 1.5);
  }

  {  // check 2D vector normal
    constexpr auto p1  = vec<2>{0., 1.};
    constexpr auto p1n = vec<2>{-1., 0.};

    constexpr auto p2  = vec<2>{1., 0.};
    constexpr auto p2n = vec<2>{0., 1.};

    CHECK(normal(p1) == p1n);
    CHECK(normal(p2) == p2n);
  }

  {  // check 3D vector cross product
    constexpr auto p1 = vec<3>{1., 0., 0.};
    constexpr auto p2 = vec<3>{0., 1., 0.};

    constexpr auto c0 = vec<3>{0., 0., 1.};
    constexpr auto c1 = vec<3>{0., 0., -1.};

    CHECK(cross(p1, p2) == c0);
    CHECK(cross(p2, p1) == c1);
  }

  return test::result();
}
