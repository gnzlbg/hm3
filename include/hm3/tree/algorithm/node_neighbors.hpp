#pragma once
/// \file node_neighbors.hpp
#include <hm3/tree/algorithm/node_location.hpp>
#include <hm3/tree/algorithm/node_or_parent_at.hpp>
#include <hm3/tree/algorithm/shift_location.hpp>
#include <hm3/tree/concepts.hpp>
#include <hm3/tree/location/default.hpp>
#include <hm3/tree/relations/neighbor.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/static_const.hpp>
#include <hm3/utility/stack/vector.hpp>

namespace hm3 {
namespace tree {
template <typename T> struct dump;

struct node_neighbors_fn {
  struct always_true_pred {
    constexpr bool operator()(node_idx) const noexcept { return true; }
  };

  /// Finds neighbors of node at location \p loc across the Manifold
  /// (appends them to a push_back-able container)
  template <typename Manifold, typename Tree, typename Loc,
            typename PushBackableContainer,
            typename UnaryPredicate = always_true_pred,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Manifold positions, Tree const& t, Loc&& loc,
                  PushBackableContainer& s,
                  UnaryPredicate&& pred = UnaryPredicate{}) const noexcept
   -> void {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    const level_idx lvl = loc.level();
    // The root node has no neighbors
    if (HM3_UNLIKELY(lvl == 0_l)) { return; }
    // For all same level neighbor positions
    for (auto&& sl_pos : positions()) {
      auto neighbor
       = node_or_parent_at(t, shift_location(loc, positions[sl_pos]));
      const auto n = neighbor.idx;
      if (!n) { continue; }
      HM3_ASSERT((neighbor.level == lvl) || (neighbor.level == (lvl - 1)),
                 "found neighbor must either be at the same level {} or at the "
                 "parent level {}, but is instead at level {}",
                 lvl, lvl - 1, neighbor.level);

      if (t.is_leaf(n)) {
        // if the neighbor found is a leaf node (and it matches the predicate)
        // we are done
        // note: it is either at the same or parent level of the node
        // (doesn't matter which case it is, it is the correct neighbor)
        if (pred(n)) {
          s.push_back(n);
        } else if (!Same<UnaryPredicate,
                         always_true_pred>()  // User-defined pred
                   and !t.is_root(n) and pred(t.parent(n))) {
          // If the leaf node doesn't match the predicate, it's parent might
          s.push_back(t.parent(n));
        }
        // if neither the leaf node nor its parent match the predicate then they
        // are not a neighbor (this happens when e.g. a grid ends in the middle
        // of the tree)
      } else {
        // if it has children we add the children sharing a face with the node
        // that satisfy the predicate
        for (auto&& cp : Manifold{}.children_sharing_face(sl_pos)) {
          auto c = t.child(n, cp);
          if (pred(c)) { s.push_back(c); }
        }
      }
    }
  }

  /// Finds neighbors of node at location \p loc across the Manifold
  ///
  /// \returns stack allocated vector containing the neighbors
  template <typename Manifold, typename Tree, typename Loc,
            typename UnaryPredicate = always_true_pred,
            int max_no_neighbors    = Manifold::no_child_level_neighbors(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Manifold, Tree const& t, Loc&& loc,
                  UnaryPredicate&& pred = UnaryPredicate()) const noexcept
   -> stack::vector<node_idx, max_no_neighbors> {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    stack::vector<node_idx, max_no_neighbors> neighbors;
    (*this)(Manifold{}, t, loc, neighbors, std::forward<UnaryPredicate>(pred));
    return neighbors;
  }

  /// Finds neighbors of node \p n across the Manifold
  ///
  /// \returns stack allocated vector containing the neighbors
  template <typename Manifold, typename Tree,
            typename Loc            = loc_t<Tree::dimension()>,
            uint_t max_no_neighbors = Manifold::no_child_level_neighbors(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Manifold, Tree const& t, node_idx n, Loc l = Loc{}) const
   noexcept -> stack::vector<node_idx, max_no_neighbors> {
    static_assert(Tree::dimension() == ranges::uncvref_t<Loc>::dimension(), "");
    return (*this)(Manifold{}, t, node_location(t, n, l));
  }

  /// Finds set of unique neighbors of node at location \p loc across all
  /// manifolds
  ///
  /// \param t [in] tree.
  /// \param loc [in] location (location of the node).
  /// \returns stack allocated vector containing the unique set of neighbors
  ///
  template <typename Tree, typename Loc,
            typename UnaryPredicate = always_true_pred,
            uint_t nd = Tree::dimension(), CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, Loc&& loc,
                  UnaryPredicate&& pred = UnaryPredicate()) const noexcept
   -> stack::vector<node_idx, max_no_neighbors(nd)> {
    stack::vector<node_idx, max_no_neighbors(nd)> neighbors;

    // For each surface manifold append the neighbors
    using manifold_rng = meta::as_list<meta::integer_range<int, 1, nd + 1>>;
    meta::for_each(manifold_rng{}, [&](auto m_) {
      using manifold = manifold_neighbors<nd, decltype(m_){}>;
      (*this)(manifold{}, t, loc, neighbors,
              std::forward<UnaryPredicate>(pred));
    });

    // sort them and remove dupplicates
    ranges::sort(neighbors);
    neighbors.erase(ranges::unique(neighbors), end(neighbors));
    return neighbors;
  }

  /// Finds set of unique neighbors of node \p n across all manifolds
  ///
  /// \param t [in] tree.
  /// \param n [in] node index.
  /// \returns stack allocated vector containing the unique set of neighbors
  ///
  template <typename Tree, typename Loc = loc_t<Tree::dimension()>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, node_idx n, Loc l = Loc()) const noexcept {
    return (*this)(t, node_location(t, n, l));
  }
};

namespace {
constexpr auto&& node_neighbors = static_const<node_neighbors_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
