/// \file
///
/// Tree 1D tests
#include "tree_1d.hpp"

using namespace hm3;
using namespace test;

/// Explicit instantiate it
template struct hm3::tree::tree<1>;

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(
 std::declval<tree<1>>().children(node_idx{}));
HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(std::declval<tree<1>>()());

HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(tree<1>::child_positions());

template <template <dim_t, class...> class Loc>
void test_tree() {
  {  // check construction
    tree<1> t(1);
    CHECK(t.capacity() == 1_u);
    CHECK(t.size() == 1_u);
    CHECK(!t.empty());
    CHECK(t.is_leaf(0_n));
  }

  {  // check capacity
    CHECK(tree<1>(1).capacity() == 1_u);
    CHECK(tree<1>(2).capacity() == 3_u);
    CHECK(tree<1>(3).capacity() == 3_u);
    CHECK(tree<1>(4).capacity() == 5_u);
    CHECK(tree<1>(14).capacity() == 15_u);
    CHECK(tree<1>(15).capacity() == 15_u);
  }
  {
    tree<1> t(20);
    CHECK(t.capacity() == 21_u);
    CHECK(t.size() == 1_u);
    CHECK(t.is_leaf(0_n));
    t.refine(0_n);
    CHECK(!t.is_leaf(0_n));
    CHECK(t.size() == 3_u);

    {  // node locs
      auto l1 = node_location(t, 1_n, Loc<1>{});
      CHECK(l1.level() == 1_u);
      CHECK(l1[1] == 0u);
      auto l2 = node_location(t, 2_n, Loc<1>{});
      CHECK(l2.level() == 1_u);
      CHECK(l2[1] == 1u);
    }
    t.refine(1_n);
    CHECK(t.size() == 5_u);

    {  // node locs
      auto l3 = node_location(t, 3_n, Loc<1>{});
      CHECK(l3.level() == 2_u);
      CHECK(l3[1] == 0_u);
      CHECK(l3[2] == 0_u);
      auto l4 = node_location(t, 4_n, Loc<1>{});
      CHECK(l4.level() == 2_u);
      CHECK(l4[1] == 0_u);
      CHECK(l4[2] == 1_u);
    }
    t.refine(2_n);
    CHECK(t.size() == 7_u);

    t.refine(3_n);
    CHECK(t.size() == 9_u);
    t.refine(4_n);
    CHECK(t.size() == 11_u);
    t.refine(5_n);
    CHECK(t.size() == 13_u);
    t.refine(6_n);
    CHECK(t.size() == 15_u);

    check_tree(t, uniform_tree{}, Loc<1>{});
    check_is_balanced(t, Loc<1>{});
    check_tree_node_range(t, uniform_tree{});
    CHECK(t.is_compact());
    check_io(t, "uniform", Loc<1>{});

    t.refine(11_n);
    CHECK(t.size() == 17_u);
    t.refine(8_n);
    CHECK(t.size() == 19_u);
    t.refine(9_n);
    CHECK(t.size() == 21_u);

    check_tree(t, tree_after_refine{}, Loc<1>{});
    check_tree_node_range(t, tree_after_refine{});
    CHECK(t.is_compact());
    check_is_balanced(t, Loc<1>{});
    check_io(t, "after_refine", Loc<1>{});

    t.coarsen(11_n);
    CHECK(t.size() == 19_u);
    CHECK(!t.is_compact());

    t.coarsen(5_n);
    CHECK(t.size() == 17_u);

    CHECK(!t.is_compact());
    check_tree(t, tree_after_coarsen{}, Loc<1>{});
    check_tree_node_range(t, tree_after_coarsen{});
    check_is_balanced(t, Loc<1>{});
    check_io(t, "after_coarsen", Loc<1>{});

    auto t2 = t;
    CHECK(t == t2);
    CHECK(!(t != t2));

    dfs_sort(t);
    CHECK(t != t2);
    CHECK(!(t == t2));
    CHECK(t.is_compact());
    CHECK(dfs_sort.is(t));
    auto tree_after_coarsen_sorted
     = rewrite_nodes(tree_after_coarsen{}, tree_after_coarsen_sorted_map);
    ranges::sort(tree_after_coarsen_sorted.nodes,
                 [](auto&& a, auto&& b) { return *a.idx < *b.idx; });
    check_tree(t, tree_after_coarsen_sorted, Loc<1>{});
    check_tree_node_range(t, tree_after_coarsen_sorted);
    check_is_balanced(t, Loc<1>{});
    check_io(t, "after_sort", Loc<1>{});
  }
}

int main() {
  test_tree<location::deinterleaved>();
  test_tree<location::interleaved>();
  return test::result();
}
