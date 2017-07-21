#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/utility/test.hpp>
#include <iomanip>

using namespace hm3;
using namespace geometry;

static_assert(!Resizable<array<int, 2>>{});
static_assert(Resizable<small_vector<int, 2>>{});

template <dim_t Ad, typename Poly, typename Vertices>
void test_poly(Poly const& p, Vertices&& vertices_, bool counter_clockwise,
               point<Ad> centroid_, num_t area_, num_t boundary_,
               aabb<Ad> aabb_, vec<3> normal_, bool random_poly_test = false) {
  static_assert(AmbientDimension<Poly>{}, "");
  static_assert(AmbientDimension<Poly, Ad>{}, "");

  static_assert(ElementDimension<Poly>{}, "");
  static_assert(ElementDimension<Poly, Ad>{}, "");
  static_assert(ElementDimension<Poly, Ad, 2>{}, "");

  static_assert(GeometryObject<Poly>{}, "");
  static_assert(GeometryObject<Poly, Ad>{}, "");
  static_assert(GeometryObject<Poly, Ad, 2>{}, "");

  static_assert(Polygon<Poly>{}, "");
  static_assert(Polygon<Poly, Ad>{}, "");

  // relational ops
  CHECK(p == p);
  CHECK(!(p != p));

  // check vertices
  {
    CHECK(equal(vertices(p), vertices_));
    CHECK(vertex_size(p) == (dim_t)ranges::distance(vertices_));
    CHECK(vertex_size(p) == (dim_t)ranges::distance(vertices(p)));
    uint_t count_ = 0;
    for (auto idx : vertex.indices(p)) {
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
    CHECK(edge_size(p) == vertex_size(p));
    test::check_equal(edge.indices(p), vertex.indices(p));
    auto create_face = [&](auto fidx) {
      auto v0 = vertex(p, fidx);
      auto v1 = vertex(p, (fidx == vertex_size(p) - 1) ? 0 : fidx + 1);
      return segment<Ad>(v0, v1);
    };
    for (auto&& v : vertex.indices(p)) {
      auto f = create_face(v);
      CHECK(edge(p, v) == f);
    }

    test::check_equal(edges(p), edge.indices(p) | view::transform(create_face));
  }

  // check vertex order
  if constexpr (Ad == 2) {
    auto order_should
     = counter_clockwise ? vertex_order_t::ccw : vertex_order_t::cw;
    CHECK(vertex_order(p) == order_should);
    if (!random_poly_test) { CHECK(vertex_order(p) == order_should); }
  }

  // check boundary
  CHECK(integral.boundary(p) == boundary_);

  if (Ad == 2 and !counter_clockwise) { return; }
  // the following operations require counter clockwise vertices in 2D
  // in 3D cw/ccw make little to no sense as a check, one order or the other
  // just means that the normal points in the opposite direction.

  // check area
  if constexpr (Ad == 2) { CHECK(integral.volume(p) == area_); }
  if constexpr (Ad == 3) { CHECK(integral.surface(p) == area_); }

  // check centroid
  CHECK(centroid(p) == centroid_);
}

template <typename Vertices>
auto make_edges(Vertices& vxs) {
  using p_t = ranges::range_value_t<Vertices>;
  using s_t = associated::edge_t<p_t>;
  return view::ints(std::size_t{0}, ranges::size(vxs))
         | view::transform([&](auto idx) {
             return s_t(vxs[idx],
                        vxs[idx == ranges::size(vxs) - 1 ? 0 : idx + 1]);
           });
}

template <typename Poly>
void test_2d_quad() {
  using p2d = point<2>;
  using q2d = Poly;
  {  // 2D unit ccw box (0):
    p2d vs[]    = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
    p2d xc      = {0.5, 0.5};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);

    q2d q(make_edges(vs));
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D unit ccw box (1):
    p2d vs[]    = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
    p2d xc      = {0.0, 0.0};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>(p2d::constant(-0.5), p2d::constant(0.5));
    auto n      = vec<3>::unit(2);
    q2d q(make_edges(vs));
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D unit ccw box (2):
    p2d vs[]    = {{-1.0, -1.0}, {0.0, -1.0}, {0.0, 0.0}, {-1.0, 0.0}};
    p2d xc      = {-0.5, -0.5};
    num_t a     = 1.0;
    num_t b     = 4.0;
    auto aabbox = aabb<2>(p2d::constant(-1.0), p2d::constant(0.0));
    auto n      = vec<3>::unit(2);
    q2d q(make_edges(vs));
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
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
    q2d q(make_edges(vs));
    test_poly<2>(q, vs, true, xc, a, b, aabbox, n);
  }
}

template <typename Poly>
void test_2d_tri() {
  using t2d = Poly;
  using p2d = associated::point_t<t2d>;

  {  // 2D ccw triangle (0):
    p2d vs[]    = {{1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(make_edges(vs));
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (1):
    p2d vs[]    = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}};
    p2d xc      = {2. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(make_edges(vs));
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (2):
    p2d vs[]    = {{1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};
    p2d xc      = {1. / 3., 2. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    auto n      = vec<3>::unit(2);
    t2d t(make_edges(vs));
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D ccw triangle (3):
    p2d vs[]    = {{0.0, 0.0}, {0.5, 0.0}, {0.5, 1.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 0.5 * 1.0;
    num_t b     = std::sqrt(1.25) + 1.5;
    auto aabbox = aabb<2>(p2d{0.0, 0.0}, p2d{0.5, 1.0});
    auto n      = vec<3>::unit(2);
    t2d t(make_edges(vs));
    test_poly<2>(t, vs, true, xc, a, b, aabbox, n);
  }

  {  // 2D cw triangle (0):
    p2d vs[]    = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}};
    p2d xc      = {1. / 3., 1. / 3.};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<2>::unit();
    vec<3> n{0.0, 0.0, -1.0};

    t2d t(make_edges(vs));
    test_poly<2>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // test: correct last point
    p2d vs1[] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.e-13, 1.e-13}};
    t2d t1(make_open_segment_range(vs1));

    p2d vs2[] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}};
    t2d t2(make_edges(vs2));
    CHECK(t1 == t2);
  }
}

template <typename Poly>
void test_3d_tri() {
  using p3d = point<3>;
  using t3d = Poly;

  {  // 3D ccw triangle (0)
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}};
    p3d xc      = {2. / 3., 1. / 3., 0.0};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>::unit(2);
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }
  {  // 3D ccw triangle (1)
    p3d vs[]    = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1.0 * 1.0;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>::unit(2);
    t3d t(make_edges(vs));
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
    t3d t(make_edges(vs));
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
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (4) yz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, {0.0, 1.0, 0.0}};
    p3d xc      = {0.0, 2. / 3., 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{-math::eps, 0., 0.}, p3d{+math::eps, 1., 1.});
    auto n      = vec<3>{-1., 0., 0.};
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (5) xy-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>{0., 0., -1.};
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xy-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
    p3d xc      = {1. / 3., 2. / 3., 0.0};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., 0., -math::eps}, p3d{1., 1., +math::eps});
    auto n      = vec<3>{0., 0., 1.};
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}};
    p3d xc      = {2. / 3., 0.0, 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., -math::eps, 0.}, p3d{1., +math::eps, 1.});
    auto n      = vec<3>{0., -1., 0.};
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }

  {  // 3D ccw triangle (6) xz-plane
    p3d vs[]    = {{0.0, 0.0, 0.0}, {1.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
    p3d xc      = {2. / 3., 0.0, 1. / 3.};
    num_t a     = 0.5 * 1. * 1.;
    num_t b     = std::sqrt(2.) + 2.0;
    auto aabbox = aabb<3>(p3d{0., -math::eps, 0.}, p3d{1., +math::eps, 1.});
    auto n      = vec<3>{0., 1., 0.};
    t3d t(make_edges(vs));
    test_poly<3>(t, vs, false, xc, a, b, aabbox, n);
  }
}

int main() {
  {  // 2D quad tests
    test_2d_quad<polygon<2>>();
    test_2d_quad<quad<2>>();
    test_2d_quad<edge_polygon<2>>();
  }
  {  // 2D tri tests
    test_2d_tri<polygon<2>>();
    test_2d_tri<triangle<2>>();
    test_2d_tri<edge_polygon<2>>();
  }
  {  // 3D tri tests
    test_3d_tri<polygon<3>>();
    test_3d_tri<triangle<3>>();
    test_3d_tri<edge_polygon<3>>();
  }

  using p2d = point<2>;
  using p3d = point<3>;

  {  // 3D hexahedron
    // put it in the xy-plane
    using hexaedron3d = fixed_polygon<3, 6>;

    p3d vs[] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
                {1.0, 2.0, 0.0}, {0.0, 2.0, 0.0}, {-1.0, 1.0, 0.0}};

    p3d xc      = {0.0, 1.0, 0.0};
    num_t a     = 4. * (0.5 * 1. * 1.);  // 4 triangles
    num_t b     = 4. * std::sqrt(2.) + 2.;
    auto aabbox = aabb<3>(p3d{-1., 0., -math::eps}, p3d{1., 2., +math::eps});
    auto n      = vec<3>{0., 0., 1.};
    hexaedron3d t(make_edges(vs));
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
        hexaedron3d t2(make_edges(vs2));
        test_poly<3>(t2, vs2, true, xc, a, b, aabbox, n2, true);
      }
    }
  }
  {  // test convex polygon
    using bp5 = polygon<2>;
    p2d ps[]  = {p2d{0.0, 0.0}, p2d{0.5, 0.0}, p2d{1.0, 0.0}, p2d{1.0, 1.0},
                p2d{0.0, 1.0}};
    bp5 p0(make_edges(ps));
    CHECK(is_convex(p0));

    p2d ps1[] = {p2d{0.0, 0.0}, p2d{0.5, 0.5}, p2d{1.0, 0.0}, p2d{1.0, 1.0},
                 p2d{0.0, 1.0}};
    bp5 p1(make_edges(ps1));
    CHECK(!is_convex(p1));
  }
  {  // test relative position
    p2d vs[] = {{1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0}};
    polygon<2> t(make_edges(vs));

    using rp = relative_position_t;
    p2d p0{-0.1, 0.5};  // out
    p2d p1{0.1, 0.5};   // in
    p2d p2{0.6, 0.6};   // out
    p2d p3{1.0, 0.0};   // inter
    p2d p4{0.5, 0.0};   // inter
    CHECK(relative_position(p0, t) == rp::outside);
    CHECK(relative_position(p1, t) == rp::inside);
    CHECK(relative_position(p2, t) == rp::outside);
    CHECK(relative_position(p3, t) == rp::intersected);
    CHECK(relative_position(p4, t) == rp::intersected);
  }
  return test::result();
}
