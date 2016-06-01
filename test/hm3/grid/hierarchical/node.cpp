/// \file
///
/// Tests hierarchical Cartesian grid node
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/point.hpp>
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

  point<Nd> pm = point<Nd>::zero();
  point<Nd> pp = point<Nd>::ones();
  point<Nd> xc = point<Nd>::constant(0.5);

  point<Nd> ppw = point<Nd>::ones();
  ppw(0)        = 1.5;

  CHECK(is_box(pm, pp));
  CHECK(is_box(pm, xc));
  CHECK(is_box(xc, pp));
  if (Nd > 1) {
    CHECK(!is_box(pm, ppw));
    CHECK(!is_box(xc, ppw));
  }

  CHECK(xc == box_centroid(pm, pp));
  CHECK(xc != box_centroid(pm, xc));
  CHECK(xc != box_centroid(xc, pp));

  CHECK(box_length(pm, pp) == 1.0);
  CHECK(box_length(pm, xc) == 0.5);
  CHECK(box_length(xc, pp) == 0.5);

  cartesian::node<Nd> n1(pm, pp);
  cartesian::node<Nd> n2(box_centroid(pm, pp), box_length(pm, pp));

  CHECK(length(n1) == length(n2));
  CHECK(volume(n1) == volume(n2));
  CHECK(dimension(n1) == Nd);
  CHECK(centroid(n1) == centroid(n2));
  CHECK(volume(box<Nd>(pm, pp)) == 1.0);

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

  CHECK(static_cast<box<Nd>>(n1) == box<Nd>(pm, pp));
}

int main() {
  test_node<1>();
  test_node<2>();
  test_node<3>();
  return test::result();
}
