/// \file point.cpp Geometry point tests
#include <hm3/geometry/point.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using namespace geometry;

  {  // check constructors and dimension:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p2 = point<2>{0., 0.};
    constexpr auto p3 = point<3>{0., 0., 0.};

    static_assert(dimension(p1) == 1, "");
    static_assert(dimension(p2) == 2, "");
    static_assert(dimension(p3) == 3, "");
  }

  {  // check values:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p3 = point<3>{0., 1., 2.};
    const auto p4     = point<3>::constant(1.);

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);
    CHECK(p4(0) == 1.);
    CHECK(p4(1) == 1.);
    CHECK(p4(2) == 1.);
  }

  {  // check length:
    constexpr auto p1a = point<1>{0.};
    constexpr auto p1b = point<1>{1.};
    constexpr auto p1c = point<1>{2.};
    const auto p1d     = point<1>::constant(1.);

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
  }

  {  // check relational operators:
    constexpr auto p1a = point<1>{0.};
    constexpr auto p1b = point<1>{1.};
    constexpr auto p3a = point<3>{0., 1., 2.};
    constexpr auto p3b = point<3>{2., 0., 1.};

    CHECK((p1a == p1a));
    CHECK((p1a != p1b));
    CHECK((p3a == p3a));
    CHECK((p3a != p3b));
  }

  {  // check constexpr:
    constexpr auto p1 = point<1>{0.};
    constexpr auto p2 = point<2>{0., 1.};
    constexpr auto p3 = point<3>{0., 1., 2.};

    CHECK(dimension(p1) == 1);
    CHECK(dimension(p2) == 2);
    CHECK(dimension(p3) == 3);

    static_assert(dimension(p1) == 1, "");
    static_assert(dimension(p2) == 2, "");
    static_assert(dimension(p3) == 3, "");

    CHECK(p1(0) == 0.);
    CHECK(p3(0) == 0.);
    CHECK(p3(1) == 1.);
    CHECK(p3(2) == 2.);

    static_assert(p1(0) == 0., "");
    static_assert(p3(0) == 0., "");
    static_assert(p3(1) == 1., "");
    static_assert(p3(2) == 2., "");
  }

  {  // check assignment:
    constexpr auto p1 = point<2>{0., 1.};
    constexpr auto p2 = p1;

    static_assert(dimension(p1) == dimension(p2), "");

    static_assert(p1(0) == 0., "");
    static_assert(p1(1) == 1., "");
    static_assert(p2(0) == 0., "");
    static_assert(p2(1) == 1., "");

    constexpr auto p3 = point<1>{1.5};
    constexpr auto p4 = p3;

    static_assert(dimension(p3) == dimension(p4), "");

    static_assert(p3(0) == 1.5, "");
    static_assert(p4(0) == 1.5, "");
  }

  return test::result();
}
