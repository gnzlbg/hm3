/// \file tree_3d.cpp
#include "tree_3d.hpp"

using namespace test;

/// Explicit instantiate it
template struct hm3::tree::tree<3>;

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(
 std::declval<tree<3>>().children(node_idx{}));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(std::declval<tree<3>>()());

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(tree<3>::child_positions());

template <template <hm3::uint_t, class...> class Loc> void test_tree() {
  {  // check construction
    tree<3> t(1);
    CHECK(t.capacity() == 1_u);
    CHECK(t.size() == 1_u);
    CHECK(!t.empty());
    CHECK(t.is_leaf(0_n));
  }

  {  // check capacity
    CHECK(tree<3>(1).capacity() == 1_u);
    CHECK(tree<3>(2).capacity() == 9_u);
    CHECK(tree<3>(3).capacity() == 9_u);
    CHECK(tree<3>(4).capacity() == 9_u);
    CHECK(tree<3>(5).capacity() == 9_u);
    CHECK(tree<3>(6).capacity() == 9_u);
    CHECK(tree<3>(7).capacity() == 9_u);
    CHECK(tree<3>(8).capacity() == 9_u);
    CHECK(tree<3>(9).capacity() == 9_u);
    CHECK(tree<3>(10).capacity() == 17_u);
    CHECK(tree<3>(11).capacity() == 17_u);
    CHECK(tree<3>(12).capacity() == 17_u);
    CHECK(tree<3>(13).capacity() == 17_u);
    CHECK(tree<3>(17).capacity() == 17_u);
    CHECK(tree<3>(18).capacity() == 25_u);
  }
  {
    tree<3> t(89);
    CHECK(t.capacity() == 89_u);
    CHECK(t.size() == 1_u);
    CHECK(t.is_leaf(0_n));
    t.refine(0_n);
    CHECK(!t.is_leaf(0_n));
    CHECK(t.size() == 9_u);

    t.refine(1_n);
    t.refine(2_n);
    t.refine(3_n);
    t.refine(4_n);
    t.refine(5_n);
    t.refine(6_n);
    t.refine(7_n);
    t.refine(8_n);
    CHECK(t.size() == 73_u);

    check_tree(t, uniform_tree{}, Loc<3>{});
    CHECK(t == uniformly_refined_tree<3>(2, 2));
    check_is_balanced(t);
    check_io(t, "uniform");

    t.refine(16_n);
    t.refine(65_n);

    CHECK(t.size() == 89_u);
    check_tree(t, tree_after_refine{}, Loc<3>{});
    CHECK(t != uniformly_refined_tree<3>(2, 3));
    check_is_balanced(t);
    check_io(t, "after_refine");

#ifdef HM3_ENABLE_VTK
    vtk::serialize(t, "tree_3d_vtk_serialization");
#endif

    t.coarsen(16_n);
    CHECK(t.size() == 81_u);
    CHECK(!t.is_compact());
    check_io(t, "after_coarsen");

    // CHECK(!t.is_compact());
    // check_tree(t, tree_after_coarsen{}, Loc<3>{});

    auto t2 = t;
    CHECK(t == t2);
    CHECK(!(t != t2));

    dfs_sort(t);
    CHECK(t != t2);
    CHECK(!(t == t2));
    CHECK(t.is_compact());
    check_is_balanced(t);
    check_io(t, "after_coarsen_sorted");

    // check_tree(t,
    //            rewrite_nodes(tree_after_coarsen{},
    //            tree_after_coarsen_sorted_map),
    //            Loc<3>{});
  }

  {
    auto t = uniformly_refined_tree<3>(2, 3);
    check_tree(t, uniform_tree{}, Loc<3>{});

    t.refine(16_n);
    t.refine(65_n);

    CHECK(t.size() == 89_u);
    check_is_balanced(t);
    check_tree(t, tree_after_refine{}, Loc<3>{});
  }
}

int main() {
  test_tree<location::fast>();
  test_tree<location::slim>();

  return test::result();
}
