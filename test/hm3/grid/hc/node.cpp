/// \file
///
/// Tests hierarchical Cartesian grid node
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/grid/hc/node.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template struct grid::hc::node<1_u>;
template struct grid::hc::node<2_u>;
template struct grid::hc::node<3_u>;

template <uint_t Nd>  //
void test_node() {
  using namespace grid;
  using namespace geometry;

  point<Nd> pm = point<Nd>::zero();
  point<Nd> pp = point<Nd>::ones();
  point<Nd> xc = point<Nd>::constant(0.5);

  point<Nd> ppw = point<Nd>::ones();
  ppw(0)        = 1.5;

  CHECK(is_square(pm, pp));
  CHECK(is_square(pm, xc));
  CHECK(is_square(xc, pp));
  if (Nd > 1) {
    CHECK(!is_square(pm, ppw));
    CHECK(!is_square(xc, ppw));
  }

  CHECK(xc == square_centroid(pm, pp));
  CHECK(xc != square_centroid(pm, xc));
  CHECK(xc != square_centroid(xc, pp));

  CHECK(square_length(pm, pp) == 1.0);
  CHECK(square_length(pm, xc) == 0.5);
  CHECK(square_length(xc, pp) == 0.5);

  hc::node<Nd> n1(pm, pp);
  hc::node<Nd> n2(square_centroid(pm, pp), square_length(pm, pp));

  CHECK(length(n1) == length(n2));
  CHECK(volume(n1) == volume(n2));
  CHECK(dimension(n1) == Nd);
  CHECK(centroid(n1) == centroid(n2));
  CHECK(volume(square<Nd>(pm, pp)) == 1.0);

  CHECK(n1 == n2);
  CHECK(!(n1 != n2));

  hc::node<Nd> n3(pm, pp, tree_node_idx{3});
  hc::node<Nd> n4(pm, pp, tree_node_idx{4});
  hc::node<Nd> n5(pm, pp, tree_node_idx{3});

  CHECK(n3 == n5);
  CHECK(n4 != n3);

  CHECK(static_cast<tree_node_idx>(n1) == tree_node_idx{});
  CHECK(static_cast<tree_node_idx>(n3) == tree_node_idx{3});
  CHECK(static_cast<tree_node_idx>(n4) == tree_node_idx{4});

  CHECK(static_cast<square<Nd>>(n1) == square<Nd>(pm, pp));
}

int main() {
  test_node<1>();
  test_node<2>();
  test_node<3>();
  return test::result();
}
