#pragma once
/// \file
///
/// Compute node neighbor algorithm
#include <hm3/tree/algorithm/node_at.hpp>
#include <hm3/tree/algorithm/shift_location.hpp>
#include <hm3/tree/concepts.hpp>
#include <hm3/tree/location/default.hpp>
#include <hm3/tree/relations/neighbor.hpp>
#include <hm3/tree/types.hpp>

namespace hm3 {
namespace tree {
/// Find neighbor \p n of node at location \p loc
///
/// Note: the manifold is associated to the neighbor index type
/// (todo: strongly type this)
///
struct node_neighbor_fn {
  template <typename Loc, typename NeighborIdx,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto location(Loc l, NeighborIdx p) const noexcept {
    static_assert(Loc::dimension() == Manifold::dimension(), "");
    return shift_location(l, Manifold{}[p]);
  }

  template <typename Tree, typename NeighborIdx,
            typename Loc      = loc_t<Tree::dimension()>,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto location(Tree const& t, node_idx n, NeighborIdx p, Loc l = Loc{}) const
   noexcept {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    static_assert(Tree::dimension() == Manifold::dimension(), "");
    return location(node_location(t, n, l), p);
  }

  template <typename Tree, typename Loc, typename NeighborIdx,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, Loc&& loc, NeighborIdx p) const noexcept
   -> node_idx {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    static_assert(Tree::dimension() == Manifold::dimension(), "");
    return node_at(t, location(loc, p));
  }

  template <typename Tree, typename NeighborIdx,
            typename Loc      = loc_t<Tree::dimension()>,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, node_idx n, NeighborIdx p, Loc l = Loc{}) const
   noexcept -> node_idx {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    static_assert(Tree::dimension() == Manifold::dimension(), "");
    return node_at(t, location(t, n, p, l));
  }
};

namespace {
constexpr auto&& node_neighbor = static_const<node_neighbor_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
