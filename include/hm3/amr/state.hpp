#pragma once
/// \file
///
/// Adaptive mesh refinement state
#include <vector>
#include <hm3/amr/action.hpp>
#include <hm3/amr/target.hpp>
#include <hm3/tree/types.hpp>

namespace hm3 {
namespace amr {

/// Adaptive mesh refinement algorithm
template <typename Target> struct state {
 private:
  /// Node idx type
  using amr_node_idx = amr_node_idx_t<Target>;

  /// State stored per node:
  /// - node index
  /// - action to be performed on that node
  ///
  /// \todo This is very space inefficient. I would rather make this intrusive
  /// somehow (via Target) which might allow this being only 2bits per node
  /// instead of 128bit per node.
  ///
  struct node {
    amr_node_idx idx;
    action action;
    operator amr_node_idx() const noexcept { return idx; }
  };
  using node_data = std::vector<node>;

  /// \name AMR Data
  ///
  /// All data members
  ///@{

  /// Target to adatp
  Target t_;
  /// AMR nodes
  node_data nodes_;

  ///@}  /// AMR Data

 public:
  state() = default;
  state(Target t) : t_{std::move(t)} {}

  /// Finds a node in a sorted node_data container using binary search
  node* find(amr_node_idx n) noexcept {
    auto r = lower_bound(nodes_, n, std::less<>{}, &node::idx);
    return r != end(nodes_) && (*r).idx == n ? &(*r) : nullptr;
  }

  /// Finds a node in a sorted node_data container using binary search
  node const* find(amr_node_idx n) const noexcept {
    return const_cast<state*>(this)->find(n);
  }

  auto const& nodes() const { return nodes_; }
  auto const& target() const { return t_; }

 private:
  /// Finds a grid node in the node data container, and filters out inactive
  /// nodes
  auto to_nodes() noexcept {
    return view::transform([&](auto&& i) { return find(i); })
           | view::filter([&](auto&& i) { return i ? true : false; })
           | view::transform([&](auto&& i) -> node& { return *i; });
  }

  auto siblings(node n) noexcept { return t_.siblings(n) | to_nodes(); }
  tree::level_idx level(node n) const noexcept { return t_.level(n); }
  auto parent(node n) const noexcept { return t_.parent(n); }

  /// Predicate/Filter for nodes to be refined
  struct to_refine {
    bool operator()(action a) const noexcept { return a == action::refine; }
    bool operator()(node n) const noexcept { return (*this)(n.action); }
    auto operator()() const noexcept {
      return view::filter([](auto&& i) { return i.action == action::refine; });
    }
  };

  /// Predicate/Filter for nodes to be coarsened
  struct to_coarsen {
    bool operator()(action a) const noexcept { return a == action::coarsen; }
    bool operator()(node n) const noexcept { return (*this)(n.action); }
    auto operator()() const noexcept {
      return view::filter([](auto&& i) { return i.action == action::coarsen; });
    }
  };

 public:
  /// Returns the node data (useful for visualizing/debugging)
  node_data const& data() const noexcept { return nodes_; }

  /// Computes the actions on every node and sorts them
  template <typename Action> void compute_actions(Action&& action) {
    const auto no_nodes = distance(t_.nodes());
    nodes_.clear();
    nodes_.reserve(no_nodes);
    RANGES_FOR (auto&& n, t_.nodes()) {
      nodes_.emplace_back(node{n, action(n)});
    }
    sort(nodes_, std::less<>{}, &node::idx);
  }

  /// Enforces 2:1 across all neighbor directions (single step)
  ///
  /// \returns True if the algorithm finishes
  ///
  /// \warning This performs a single step of the 2:1 algorithm, which is
  ///          an iterative algorithm.
  bool enforce_2_to_1_ratio_step() {
    bool done = true;

    /// Step 1: Remove coarsening flags (sibling group local):
    /// - Only those sibling groups where all siblings:
    ///  - have no children, and
    ///  - are marked for coarsening
    ///  Can be coarsened (set them to nothing otherwise)
    for (auto&& n : nodes_ | to_coarsen{}()) {
      // If all siblings are at the same level and marked for coarsening: done
      if (distance(siblings(n)) == t_.no_siblings()
          and all_of(siblings(n), to_coarsen{})) {
        continue;
      }

      // Otherwise: remove coarsening flags
      for_each(siblings(n), [&](auto&& s) {
        if (s.action == action::coarsen) { s.action = action::none; }
      });
      done = false;
    }
    t_.log("after step 1 done = {}", done);

    /// Step 2: Remove coarsening flags (between sibling groups):
    /// - A sibling group marked for coarsening cannot have a neighbor that
    /// is marked for refinement since this would break 2:1
    ///
    /// note: if a node is marked for coarsening here, all its siblings
    /// are marked for coarsening too due to Step 1
    ///
    /// Or if any of the siblings is at a level greater than the node but not
    /// marked for coarsening
    for (auto&& n : nodes_ | to_coarsen{}()) {
      HM3_ASSERT(all_of(siblings(n), to_coarsen{}), "node {}", n.idx);

      if (any_of(siblings(n), [&](auto&& i) {
            auto ns = t_.neighbors(i);
            // Neighbor at the same level marked for refinement:
            return any_of(ns | to_nodes() | view::filter([&](auto&& j) {
                            return level(n) == level(j);
                          }),
                          to_refine{})
                   // Neighbor at a level higher not marked for coarsening:
                   || any_of(ns | to_nodes() | view::filter([&](auto&& j) {
                               return level(n) + 1 == level(j);
                             }),
                             [&](auto&& j) { return !to_coarsen{}(j); });
          })) {
        for_each(siblings(n), [&](auto&& s) { s.action = action::none; });
        done = false;
      }
    }
    t_.log("after step 2 done = {}", done);

    /// Step 3: Balance future refined nodes
    /// Every neighbor of a marked-for-refinement node that it is located
    /// at its parent level must also be marked for refinement to ensure 2:1
    for (auto&& n : nodes_ | to_refine{}()) {
      auto node_level = level(n);
      auto ns         = t_.neighbors(n);
      for (auto&& neighbor : ns | to_nodes()) {
        if (level(neighbor) < node_level
            and neighbor.action != action::refine) {
          neighbor.action = action::refine;
          for_each(siblings(neighbor),
                   [&](auto&& s) { s.action = action::refine; });
          done = false;
        }
      }
    }
    t_.log("after step 3 done = {}", done);
    return done;
  }

  /// Enforces 2:1 across all neighbor directions
  ///
  /// This is an iterative process
  template <typename AfterIt = std::function<void()>>
  int enforce_2_to_1_ratio(AfterIt&& after_it = []() {}) {
    int amr_iterations = 0;
    do {
      t_.log("amr iteration: {}", amr_iterations);
      after_it();
      amr_iterations++;
    } while (not enforce_2_to_1_ratio_step());
    return amr_iterations;
  }

  /// Applies the refinement and coarsening to the target's grid
  auto apply() {
    struct {
      bool target_changed;
      uint_t no_nodes_refined;
      uint_t no_nodes_coarsened;
    } result{false, 0, 0};

    // Coarse nodes
    for (auto&& n : nodes_ | to_coarsen{}()) {
      // coarse nodes only once
      for_each(siblings(n), [&](auto&& s) { s.action = action::none; });

      // coarsen node to its parent
      t_.coarsen_siblings_of(n);
      result.no_nodes_coarsened++;
      result.target_changed = true;
    }

    // Refine nodes
    for (auto&& n : nodes_ | to_refine{}()) {
      t_.refine(n);
      result.no_nodes_refined++;
      result.target_changed = true;
    }

    return result;
  }

  /// Adapts the target \p t
  template <typename Action, typename AfterIt = std::function<void()>>
  bool adapt(Action&& action, AfterIt&& after_it = []() {}) {
    t_.log("AMR started...");

    // Compute and sort actions:
    compute_actions(std::forward<Action>(action));

    // Enforces 2:1 across all neighbor directions
    auto no_amr_iterations
     = enforce_2_to_1_ratio(std::forward<AfterIt>(after_it));

    // Modify the target's grid
    auto result = apply();

    auto no_nodes_before = nodes_.size();
    auto no_nodes_after
     = no_nodes_before
       + (result.no_nodes_refined - result.no_nodes_coarsened)
          * t_.no_siblings();

    t_.log("...AMR finished!");
    t_.log("\nAMR Summary:\n"
           " - target changed?: {}\n"
           " - #of nodes before/after: {}/{}\n"
           " - #of nodes refined/coarsened: {}/{}\n"
           " - #of amr iterations: {}\n",
           result.target_changed, no_nodes_before, no_nodes_after,
           result.no_nodes_refined, result.no_nodes_coarsened,
           no_amr_iterations);
    return result.target_changed;
  }
};

/// Creates an AMR handler for a handler
///
/// It looks for the function make_amr_target in the namespace of the Targetable
template <typename Targetable, CONCEPT_REQUIRES_(!std::is_const<Targetable>{})>
auto make(Targetable& t) {
  using target_t = ranges::uncvref_t<decltype(make_amr_target(t))>;
  return state<target_t>{make_amr_target(t)};
}

}  // namespace amr
}  // namespace hm3
