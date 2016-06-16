/// \file
///
/// Tree 2D tests
#include "tree_2d.hpp"

using namespace hm3;
using namespace test;

/// Explicit instantiate it
template struct hm3::tree::tree<2>;

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(
 std::declval<tree<2>>().children(node_idx{}));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(std::declval<tree<2>>()());

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(tree<2>::child_positions());

template <template <dim_t, class...> class Loc> void test_tree() {
  {  // check construction
    tree<2> t(1);
    CHECK(t.capacity() == 1_u);
    CHECK(t.size() == 1_u);
    CHECK(!t.empty());
    CHECK(t.is_leaf(0_n));
  }

  {  // check capacity
    CHECK(tree<2>(1).capacity() == 1_u);
    CHECK(tree<2>(2).capacity() == 5_u);
    CHECK(tree<2>(3).capacity() == 5_u);
    CHECK(tree<2>(4).capacity() == 5_u);
    CHECK(tree<2>(5).capacity() == 5_u);
    CHECK(tree<2>(6).capacity() == 9_u);
    CHECK(tree<2>(7).capacity() == 9_u);
    CHECK(tree<2>(8).capacity() == 9_u);
    CHECK(tree<2>(9).capacity() == 9_u);
    CHECK(tree<2>(10).capacity() == 13_u);
    CHECK(tree<2>(11).capacity() == 13_u);
    CHECK(tree<2>(12).capacity() == 13_u);
    CHECK(tree<2>(13).capacity() == 13_u);
  }
  {
    tree<2> t(29);
    CHECK(t.capacity() == 29_u);
    CHECK(t.size() == 1_u);
    CHECK(t.is_leaf(0_n));
    t.refine(0_n);
    CHECK(!t.is_leaf(0_n));
    CHECK(t.size() == 5_u);

    t.refine(1_n);
    t.refine(2_n);
    t.refine(3_n);
    t.refine(4_n);

    CHECK(t.size() == 21_u);
    check_tree(t, uniform_tree{}, Loc<2>{});
    CHECK(t == uniformly_refined_tree<2>(2, 3));
    check_tree_node_range(t, uniform_tree{});
    check_is_balanced(t);
    check_io(t, "uniform");

    t.refine(8_n);
    t.refine(17_n);

    CHECK(t.size() == 29_u);
    check_tree(t, tree_after_refine{}, Loc<2>{});
    CHECK(t != uniformly_refined_tree<2>(2, 3));
    CHECK(t.is_compact());
    check_tree_node_range(t, tree_after_refine{});
    check_is_balanced(t);
    check_io(t, "after_refine");

    t.coarsen(2_n);
    CHECK(t.size() == 25_u);
    CHECK(!t.is_compact());

    t.coarsen(3_n);
    CHECK(t.size() == 21_u);
    CHECK(!t.is_compact());

    check_tree(t, tree_after_coarsen{}, Loc<2>{});
    check_tree_node_range(t, tree_after_coarsen{});
    check_io(t, "after_coarsen");

    auto t2 = t;
    CHECK(t == t2);
    CHECK(!(t != t2));

#ifdef HM3_ENABLE_VTK
    ::hm3::tree::vtk::serialize(t, "tree_2d_vtk_serialization");
#endif

    dfs_sort(t);
    CHECK(t != t2);
    CHECK(!(t == t2));
    CHECK(t.is_compact());
    auto tree_after_coarsen_sorted
     = rewrite_nodes(tree_after_coarsen{}, tree_after_coarsen_sorted_map);
    ranges::sort(tree_after_coarsen_sorted.nodes,
                 [](auto&& a, auto&& b) { return *a.idx < *b.idx; });
    check_tree(t, tree_after_coarsen_sorted, Loc<2>{});
    check_tree_node_range(t, tree_after_coarsen_sorted);
    check_io(t, "after_sort");

#ifdef HM3_ENABLE_VTK
    ::hm3::tree::vtk::serialize(t, "tree_2d_sorted_vtk_serialization");
#endif
  }

  {
    auto t = uniformly_refined_tree<2>(2, 3);
    check_tree(t, uniform_tree{}, Loc<2>{});

    t.refine(8_n);
    t.refine(17_n);

    CHECK(t.size() == 29_u);
    check_tree(t, tree_after_refine{}, Loc<2>{});
  }
}

int main() {
  test_tree<location::interleaved>();
  test_tree<location::deinterleaved>();

  return test::result();
}
