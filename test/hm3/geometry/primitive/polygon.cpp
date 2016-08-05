#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/utility/test.hpp>
#include <iomanip>

using namespace hm3;
using namespace geometry;

template <dim_t Nd, typename Poly, typename Vertices>
void test_poly(Poly const& p, Vertices&& vertices_, bool counter_clockwise,
               point<Nd> centroid_, num_t area_, num_t boundary_,
               aabb<Nd> aabb_, vec<3> normal_, bool random_poly_test = false) {
  // relational ops
  CHECK(p == p);
  CHECK(!(p != p));

  // check vertices
  {
    CHECK(equal(vertices(p), vertices_));
    CHECK(vertex_size(p) == (dim_t)ranges::distance(vertices_));
    CHECK(vertex_size(p) == (dim_t)ranges::distance(vertices(p)));
    uint_t count_ = 0;
    for (auto idx : vertex_indices(p)) {
      CHECK(vertex(p, idx) == vertices_[idx]);
      ++count_;
    }
    CHECK(vertex_size(p) == count_);
  }

  if (!random_poly_test) {
    // check bounding volume
    CHECK(bounding_volume.aabb(p) == aabb_);
    CHECK(bounding_volume.box(p) == bounding_volume.box(aabb_));
  }

  // check normal:
  CHECK(normal(p) == normal_);
  CHECK(normal_().norm() == 1.0);

  // check edges
  {
    CHECK(face_size(p) == vertex_size(p));
    test::check_equal(face_indices(p), vertex_indices(p));
    auto create_face = [&](auto fidx) {
      auto v0 = vertex(p, fidx);
      auto v1 = vertex(p, (fidx == vertex_size(p) - 1) ? 0 : fidx + 1);
      return segment<Nd>::through(v0, v1);
    };
    for (auto&& v : vertex_indices(p)) {
      auto f = create_face(v);
      CHECK(face(p, v) == f);
    }

    test::check_equal(faces(p), face_indices(p) | view::transform(create_face));
  }

  // check vertex order
  CHECK(vertex_order.counter_clockwise(p) != counter_clockwise);
  if (!random_poly_test) {
    CHECK(vertex_order.counter_clockwise(p) == counter_clockwise);
    CHECK(vertex_order.clockwise(p) == (!counter_clockwise));
  }

  // check boundary
  CHECK(surface_area(p) == boundary_);

  if (Nd == 2 and !counter_clockwise) { return; }
  // the following operations require counter clockwise vertices in 2D
  // in 3D cw/ccw make little to no sense as a check, one order or the other
  // just means that the normal points in the opposite direction.

  // check area
  using area_rank = integral_rank<2>;
  CHECK(integral(p, area_rank{}) == area_);

  // check centroid
  TOL_CHECK(1e-15, centroid(p) == centroid_);
}

int main() {
  using p2d = point<2>;
  using p3d = point<3>;
  using q2d = quad<2>;
  using t2d = triangle<2>;
  using t3d = triangle<3>;

  {  // 2D unit ccw box (0):
    p2d vs[]    = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
    p2d xc      = {0.5, 0.5};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);

    q2d q(vs);
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D unit ccw box (1):
    p2d vs[]    = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
    p2d xc      = {0.0, 0.0};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>(p2d::constant(-0.5), p2d::constant(0.5));
    auto n      = vec<3>::unit(2);
    q2d q(vs);
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D unit ccw box (2):
    p2d vs[]    = {{-1.0, -1.0}, {0.0, -1.0}, {0.0, 0.0}, {-1.0, 0.0}};
    p2d xc      = {-0.5, -0.5};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>(p2d::constant(-1.0), p2d::constant(0.0));
    auto n      = vec<3>::unit(2);
    q2d q(vs);
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (0):
    p2d vs[]    = {{1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(vs);
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (1):
    p2d vs[]    = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}};
    p2d xc      = {2. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(vs);
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (2):
    p2d vs[]    = {{1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};
    p2d xc      = {1. / 3., 2. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(vs);
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (3):
    p2d vs[]    = {{0.0, 0.0}, {0.5, 0.0}, {0.5, 1.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 0.5 * 1.0;
    num_t b     = std::sqrt(1.25) + 1.5;
    auto aabbox = aabb<2>(p2d{0.0, 0.0}, p2d{0.5, 1.0});
    auto n      = vec<3>::unit(2);
    t2d t(vs);
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D cw triangle (0):
    p2d vs[]    = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    vec<3> n{0.0, 0.0, -1.0};

    t2d t(vs);
    test_poly<2>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 2D ccw quad 0:
    p2d vs[] = {{0.0, 0.0}, {1.0, 0.0}, {2.0, 1.0}, {0.0, 1.0}};
    // p2d xc     = {(0.5 * 1. + (1. + 1. / 3.) * 0.5) / (1 + 0.5),
    //               (0.5 * 1 + (1. - 1. / 3.) * 0.5) / (1 + 0.5)};
    p2d xc      = {0.7777777777777778, 0.5555555555555556};
    num_t a     = 1.5;
    num_t b     = std::sqrt(2) + 4.;
    auto aabbox = aabb<2>(p2d::constant(0.0), p2d{2.0, 1.0});
    auto n      = vec<3>::unit(2);
    q2d q(vs);
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (0)
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}};
    p3d xc      = {2. / 3., 1. / 3., 0.0};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>::unit(2);
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }
  {  // 3D ccw triangle (1)
    p3d vs[]    = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>::unit(2);
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }
  {  // 3D ccw triangle (2)
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, -1.0}, {1.0, 1.0, 1.0}};
    p3d xc      = {2. / 3., 1. / 3., 0.0};
    num_t a     = std::sqrt(3. / 2.);
    num_t b     = std::sqrt(2.) + std::sqrt(3.) + std::sqrt(5.);
    auto aabbox = aabb<3>(p3d{0., 0., -1.0}, p3d{1., 1., 1.});
    auto n      = vec<3>{1. / std::sqrt(6.), -std::sqrt(2.) / std::sqrt(3.),
                    1. / std::sqrt(6.)};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }
  // test triangles that lie on the cartesian planes (important because their
  // AABBs might have zero length in one direction)
  {  // 3D ccw triangle (3) yz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 1.0}};
    p3d xc      = {0.0, 2. / 3., 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{-math::eps, 0., 0.}, p3d{+math::eps, 1., 1.});
    auto n      = vec<3>{1., 0., 0.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (4) yz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, {0.0, 1.0, 0.0}};
    p3d xc      = {0.0, 2. / 3., 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{-math::eps, 0., 0.}, p3d{+math::eps, 1., 1.});
    auto n      = vec<3>{-1., 0., 0.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (5) xy-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>{0., 0., -1.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xy-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>{0., 0., 1.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}};
    p3d xc      = {2. / 3., 0.0, 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., -math::eps, 0.}, p3d{1., +math::eps, 1.});
    auto n      = vec<3>{0., -1., 0.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
    p3d xc      = {2. / 3., 0.0, 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., -math::eps, 0.}, p3d{1., +math::eps, 1.});
    auto n      = vec<3>{0., 1., 0.};
    t3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D hexahedron
    // put it in the xy-plane
    using hexaedron3d = geometry::fixed_polygon<3, 6>;

    p3d vs[] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
                {1.0, 2.0, 0.0}, {0.0, 2.0, 0.0}, {-1.0, 1.0, 0.0}};

    p3d xc      = {0.0, 1.0, 0.0};
    num_t a     = 4. * (0.5 * 1. * 1.);  // 4 triangles
    num_t b     = 4. * std::sqrt(2.) + 2.;
    auto aabbox = aabb<3>(p3d{-1., 0., -math::eps}, p3d{1., 2., +math::eps});
    auto n      = vec<3>{0., 0., 1.};
    hexaedron3d t(vs);
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);

    // auto tran
    for (dim_t d = 0; d < 3; ++d) {
      num_t angles[] = {0., .25, .5, .75, 1.0, 1.25, 1.5, 1.75, 2.};
      for (dim_t angle = 0; angle < size(angles); ++angle) {
        auto rot_axis = vec<3>::unit(d);
        auto rot      = Eigen::AngleAxis<num_t>(angle * math::pi, rot_axis());

        p3d vs2[6];
        // apply rotation around centroid
        for (int i = 0; i < 6; ++i) { vs2[i] = rot * (vs[i]() - xc()) + xc(); }
        vec<3> n2(rot * n());
        hexaedron3d t2(vs2);
        test_poly<3>(t2, vs2, true, xc, a, b, aabbox, n2, true);
      }
    }
  }
  {
    using bp3 = geometry::bounded_polygon<2, 3>;
    using bp4 = geometry::bounded_polygon<2, 3>;
    p2d vs[]  = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 0.0}};
    t2d t(vs);

    bp3 b0(t);
    bp4 b1(t);
  }

  return test::result();
}
