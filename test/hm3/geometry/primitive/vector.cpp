/// \file
///
/// Geometry vector tests.
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/io/ascii.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::vec_primitive::vec<1>;
template struct hm3::geometry::vec_primitive::vec<2>;
template struct hm3::geometry::vec_primitive::vec<3>;

template <typename T, dim_t Ad>
void check_vector_concepts() {
  static_assert(AmbientDimension<T>{}, "");
  static_assert(AmbientDimension<T, Ad>{}, "");

  static_assert(ElementDimension<T>{}, "");
  static_assert(ElementDimension<T, Ad>{}, "");
  static_assert(ElementDimension<T, Ad, 1>{}, "");

  static_assert(GeometryObject<T>{}, "");
  static_assert(GeometryObject<T, Ad>{}, "");
  static_assert(GeometryObject<T, Ad, 1>{}, "");

  static_assert(Vector<T>{}, "");
  static_assert(Vector<T, Ad>{}, "");
}

int main() {
  using namespace geometry;

  check_vector_concepts<vec<1>, 1>();
  check_vector_concepts<vec<2>, 2>();
  check_vector_concepts<vec<3>, 3>();

  {  // check constructors and dimension:
    constexpr auto p1 = vec<1>{0.};
    constexpr auto p2 = vec<2>{0., 0.};
    constexpr auto p3 = vec<3>{0., 0., 0.};

    STATIC_CHECK(ambient_dimension(p1) == 1_su);
    STATIC_CHECK(ambient_dimension(p2) == 2_su);
    STATIC_CHECK(ambient_dimension(p3) == 3_su);
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

    CHECK(integral.path(p1a) == 0.);
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

    CHECK(p1a == p1a);
    CHECK(p1a != p1b);
    CHECK(p3a == p3a);
    CHECK(p3a != p3b);
  }

  {  // check constexpr:
    constexpr auto p1 = vec<1>{0.};
    constexpr auto p2 = vec<2>{0., 1.};
    constexpr auto p3 = vec<3>{0., 1., 2.};

    STATIC_CHECK(ambient_dimension(p1) == 1_su);
    STATIC_CHECK(ambient_dimension(p2) == 2_su);
    STATIC_CHECK(ambient_dimension(p3) == 3_su);

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p3(0) == 0.);
    STATIC_CHECK(p3(1) == 1.);
    STATIC_CHECK(p3(2) == 2.);
  }

  {  // check assignment:
    constexpr auto p1 = vec<2>{0., 1.};
    constexpr auto p2 = p1;

    STATIC_CHECK(ambient_dimension(p1) == ambient_dimension(p2));

    STATIC_CHECK(p1(0) == 0.);
    STATIC_CHECK(p1(1) == 1.);
    STATIC_CHECK(p2(0) == 0.);
    STATIC_CHECK(p2(1) == 1.);

    constexpr auto p3 = vec<1>{1.5};
    constexpr auto p4 = p3;

    STATIC_CHECK(ambient_dimension(p3) == ambient_dimension(p4));

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

  {  // check parallel and direction
    constexpr auto v0 = vec<1>{1.};
    constexpr auto v1 = vec<1>{2.};
    CHECK(parallel(v0, v1));

    constexpr auto v2 = vec<2>{0., 1.};
    constexpr auto v3 = vec<2>{0., 2.};
    constexpr auto v4 = vec<2>{1., 1.};
    constexpr auto v5 = vec<2>{1., 0.};
    constexpr auto v6 = vec<2>{2., 2.};

    CHECK(parallel(v2, v3));
    CHECK(not parallel(v2, v4));
    CHECK(not parallel(v2, v5));
    CHECK(not parallel(v4, v5));
    CHECK(parallel(v4, v4));
    CHECK(parallel(v4, v6));

    constexpr auto v7  = vec<3>{0., 1., 0.};
    constexpr auto v8  = vec<3>{0., 2., 0.};
    constexpr auto v9  = vec<3>{1., 1., 1.};
    constexpr auto v10 = vec<3>{1., 0., 0.};
    constexpr auto v11 = vec<3>{2., 2., 2.};

    CHECK(parallel(v7, v8));
    CHECK(not parallel(v7, v9));
    CHECK(not parallel(v7, v10));
    CHECK(not parallel(v9, v10));
    CHECK(parallel(v9, v9));
    CHECK(parallel(v9, v11));

    // check direction:
    CHECK(direction(v0) == v0);
    CHECK(direction(v2) == v2);
    CHECK(direction(v7) == v7);

    CHECK(direction.invert(v0) == vec<1>{-1. * v0()});
    CHECK(direction.invert(v2) == vec<2>{-1. * v2()});
    CHECK(direction.invert(v7) == vec<3>{-1. * v7()});
  }

  return test::result();
}
