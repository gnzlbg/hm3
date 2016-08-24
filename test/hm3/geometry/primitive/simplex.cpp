#include <hm3/geometry/primitive/simplex.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using namespace hm3;
  using namespace geometry;

  {  // 1D (points)
    using point_t = point<1>;
    using vec_t   = vec<1>;
    point_t p[]   = {{.5}};
    simplex<1> s(p);

    // Vertices:
    CHECK(vertex(s, 0) == p[0]);
    CHECK(equal(vertices(s), p));

    // Centroid
    CHECK(centroid(s) == p[0]);

    // Normal
    CHECK(normal(s) == vec_t{1.0});

    // Bounding box
    auto sbb  = bounding_volume.box(s);
    auto aabb = bounding_volume.aabb(s);
    CHECK(sbb == box<1>(x_min(aabb), x_max(aabb)));
    CHECK(centroid(sbb) == centroid(s));
    CHECK(centroid(aabb) == centroid(s));
  }

  {  // 2D (lines)
    using point_t = point<2>;
    using vec_t   = vec<2>;
    using box_t   = geometry::box<2>;
    using aabb_t  = aabb<2>;
    point_t p[]   = {{0.0, 0.0}, {1.0, 1.0}};
    simplex<2> s(p);

    // Vertices:
    CHECK(vertex(s, 0) == p[0]);
    CHECK(vertex(s, 1) == p[1]);
    CHECK(equal(vertices(s), p));

    // Centroid:
    CHECK(centroid(s) == point_t{0.5, 0.5});

    // Normal:
    CHECK(normal(s) == vec_t{-1. / std::sqrt(2.), 1. / std::sqrt(2.)});

    // Bounding box
    auto sbb_s = box_t(point_t{.5, .5}, 1.);
    auto sbb   = bounding_volume.box(s);
    CHECK(sbb == sbb_s);
    auto aabb_s = aabb_t(point_t{.0, .0}, point_t{1., 1.});
    auto aabb   = bounding_volume.aabb(s);
    CHECK(aabb == aabb_s);
    CHECK(centroid(sbb) == centroid(s));
    CHECK(centroid(aabb) == centroid(s));
    {  // a skew line
      point_t p2[] = {{0.0, 0.0}, {1., .5}};
      simplex<2> s2(p2);
      auto sbb2_s = box_t(point_t{.5, .25}, 1.);
      auto sbb2   = bounding_volume.box(s2);
      CHECK(sbb2 == sbb2_s);

      auto aabb2_s = aabb_t(point_t{.0, .0}, point_t{1., .5});
      auto aabb2   = bounding_volume.aabb(s2);
      CHECK(aabb2 == aabb2_s);
    }
  }

  {  // 3D (triangles)
    using point_t = point<3>;
    point_t p[]   = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}};
    simplex<3> s(p);

    // Bounding box
    // Vertices:
    CHECK(vertex(s, 0) == p[0]);
    CHECK(vertex(s, 1) == p[1]);
    CHECK(equal(vertices(s), p));

    // Centroid:
    CHECK(centroid(s) == point_t{2. / 3., 1. / 3., 0.});
  }

  return test::result();
}
