/// \file
///
/// Tests hierarchical Cartesian grid node
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/cartesian/node.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template struct hierarchical::cartesian::node<1_u>;
template struct hierarchical::cartesian::node<2_u>;
template struct hierarchical::cartesian::node<3_u>;

template <uint_t Nd>  //
void test_node() {
  using namespace grid;
  using namespace geometry;
  using namespace hierarchical;
  using p_t = point<Nd>;
  using a_t = aabb<Nd>;
  using b_t = geometry::box<Nd>;

  p_t pm = p_t::zero();
  p_t pp = p_t::ones();
  p_t xc = p_t::constant(0.5);

  p_t ppw = p_t::ones();
  ppw(0)  = 1.5;

  CHECK(is_box(a_t(pm, pp)));
  CHECK(is_box(a_t(pm, xc)));
  CHECK(is_box(a_t(xc, pp)));
  if (Nd > 1) {
    CHECK(!is_box(a_t(pm, ppw)));
    CHECK(!is_box(a_t(xc, ppw)));
  }

  CHECK(xc == centroid(b_t(pm, pp)));
  CHECK(xc != centroid(b_t(pm, xc)));
  CHECK(xc != centroid(b_t(xc, pp)));

  CHECK(geometry::bounding_length(b_t(pm, pp)) == 1.0);
  CHECK(geometry::bounding_length(b_t(pm, xc)) == 0.5);
  CHECK(geometry::bounding_length(b_t(xc, pp)) == 0.5);

  cartesian::node<Nd> n1(pm, pp);
  cartesian::node<Nd> n2(centroid(b_t(pm, pp)),
                         geometry::bounding_length(b_t(pm, pp)));

  CHECK(geometry::bounding_length(n1) == geometry::bounding_length(n2));
  CHECK(volume(n1) == volume(n2));
  CHECK(dimension(n1) == Nd);
  CHECK(centroid(n1) == centroid(n2));
  CHECK(volume(geometry::box<Nd>(pm, pp)) == 1.0);

  CHECK(n1 == n2);
  CHECK(!(n1 != n2));

  cartesian::node<Nd> n3(pm, pp, tree_node_idx{3});
  cartesian::node<Nd> n4(pm, pp, tree_node_idx{4});
  cartesian::node<Nd> n5(pm, pp, tree_node_idx{3});

  CHECK(n3 == n5);
  CHECK(n4 != n3);

  CHECK(static_cast<tree_node_idx>(n1) == tree_node_idx{});
  CHECK(static_cast<tree_node_idx>(n3) == tree_node_idx{3});
  CHECK(static_cast<tree_node_idx>(n4) == tree_node_idx{4});

  CHECK(static_cast<geometry::box<Nd>>(n1) == geometry::box<Nd>(pm, pp));
}

int main() {
  test_node<1>();
  test_node<2>();
  test_node<3>();
  return test::result();
}
