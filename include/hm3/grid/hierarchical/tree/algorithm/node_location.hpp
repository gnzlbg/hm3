#pragma once
/// \file
///
/// Compute node location algorithm
#include <hm3/grid/hierarchical/tree/algorithm/root_traversal.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/location/default.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/assert.hpp>

namespace hm3::tree {

struct node_location_fn {
  /// Location code of the node with index \p n within the tree \p t
  template <typename Tree, typename Loc = loc_t<ad_v<Tree>>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, node_idx n, Loc loc = Loc()) const noexcept
   -> Loc {
    HM3_ASSERT(n, "cannot compute the location of an invalid node");
    auto l = [&](node_idx i) {
      //assert(((std::size_t)((void*)(this)) % 8 == 0) && "????");
      if (t.is_root(i)) { return false; }
      loc.push(Tree::position_in_parent(i));
      return true;
    };
    static_assert(alignof(decltype(l)) == 8, "??");
    //    assert(((std::size_t)((void*)(&l)) % 8 == 0) && "????");
    root_traversal(t, n, l);
    loc.reverse();
    return loc;
  }
};

namespace {
constexpr auto const& node_location = static_const<node_location_fn>::value;
}  // namespace

}  // namespace hm3::tree
