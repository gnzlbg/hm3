#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Implements a n-dimensional Hyperoctree
////////////////////////////////////////////////////////////////////////////////
// Include:
#include <hom3/core.hpp>
#include <hom3/core/matrix.hpp>
#include <hom3/io/data_file.hpp>
#include <fstream>
////////////////////////////////////////////////////////////////////////////////
// File macros:
#define ENABLE_DBG_ 0
#include <hom3/core/dbg.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

/// n-dimensional Hyperoctree implementation
namespace hyperoctree {

struct empty_data_layout {
  /// The data container type
  struct empty {};
  using data = empty;
  /// How to construct the data container
  static inline data make(const Ind) { return data{}; }
  /// How to get a reference to the node data
  static inline data& get_ref(data&, const Ind) { return dummy_; }
  /// How to get a reference to the node data
  static inline data const& get_ref(data const&, const Ind) { return dummy_; }
  /// How to get a value of the node data
  static inline data get_val(data const&, const Ind) { return dummy_; }
  /// How to set a node data
  static inline void set(data&, const Ind, data const&) {}

 private:
  static data dummy_;
};

template <SInd nd_, class DataLayout = empty_data_layout> struct hyperoctree {
  //////////////////////////////////////////////////////////////////////////////
  /// \name Types, Tags, and Metadata
  ///@{
 public:
  static const constexpr SInd nd = nd_;  ///< \f$\#\f$ of space dimensions

 protected:
  using data_layout = DataLayout;

 private:
  using this_type = hyperoctree<nd, data_layout>;
  static_assert(nd >= 1 && nd < 4,  // in range: [1,4)
                "unimplemented number of spatial dimensions");

 public:
  /// \name Neighbor set tags
  ///@{

  /// Set of neighbors at the same level
  struct at_same_level_t {};
  static const constexpr at_same_level_t at_same_level{};

  /// Set of all neighbors (across levels, smallest neighbor chosen)
  struct all_neighbors_t {};
  static const constexpr all_neighbors_t all_neighbors{};

  ///@}  // Neighbor set tags

  /// \name Neighbor selectors
  ///@{

  /// Selects m neighbors (m = nd - i)
  template <SInd m> using across = integral::t<SInd, m>;

  /// Selects all neigbhors in range [0, nd)
  struct across_all_t {};
  static const constexpr across_all_t across_all{};

  /// Range [0, nd)
  using across_all_range = integral::range_c<SInd, 0, nd>;

  using across_faces_t = across<nd - 1>;                 ///< For faces
  static const constexpr across_faces_t across_faces{};  ///< For faces

  using across_edges_t = across<nd - 2>;                 ///< For edges
  static const constexpr across_edges_t across_edges{};  ///< For edges

  using across_corners_t = across<nd - 3>;                   ///< For Corners
  static const constexpr across_corners_t across_corners{};  ///< For Corners

  using neighbor_types
   = integral::list<across_faces_t, across_edges_t, across_corners_t>;

  ///@}  // Neighbor selectors

  //////////////////////////////////////////////////////////////////////////////
  /// \name Types
  ///@{
 private:
  struct node_idx_tag {};
  struct sibling_group_idx_tag {};
  struct child_pos_idx_tag {};
  struct nghbr_pos_idx_tag {};

  template <class T, class Idx, Int nRows = dense::dynamic>
  using vector = dense::vector<T, nRows, Idx>;

  template <class Idx, Int nRows = dense::dynamic>
  using bit_vector = dense::bit_vector<nRows, Idx>;

  /// Sibling Group Idx
  using sibling_group_idx = arithmetic<Ind, sibling_group_idx_tag>;

 public:
  using node_idx = arithmetic<Ind, node_idx_tag>;         ///< Node idx
  using child_pos = arithmetic<SInd, child_pos_idx_tag>;  ///< Child pos idx
  using neighbor_pos = arithmetic<SInd, nghbr_pos_idx_tag>;  ///< Neighbor pos idx

  template <SInd noNeighbors>
  using neighbor_ids = vector<node_idx, neighbor_pos, noNeighbors>;

  ///@}  // Types
  //////////////////////////////////////////////////////////////////////////////

  ///@}  // Types, Tags, and Metadata
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Encoded spatial information
  ///@{
 public:
  /// Number of siblings within a sibling group
  static constexpr SInd no_siblings() noexcept {
    return math::ct::ipow(2u, nd);
  }
  /// Number of children per node
  static constexpr SInd no_children() noexcept { return no_siblings(); }


  /// Array of children:
  using ChildIdxA = vector<node_idx, child_pos, no_children()>;


  /// Max allowed\f$\#\f$ of levels
  static constexpr SInd max_no_levels() noexcept { return 20; }

  /// \name Maximum number of neighbors
  ///@{

 private:
  /// TypeClass Foldable
  template <class T> struct Foldable : T {
    /// fold
    template <class U>
    constexpr int operator()(const SInd& result, const U t) const noexcept {
      return result + T{}(t);
    }

    /// join/flatten: across<m> -> m
    template <SInd m> constexpr SInd operator()(across<m>) const noexcept {
      return T{}(m);
    }

    using T::operator();
  };

  /// \f$\#\f$ of (nd - m) neighbors at the same level
  ///
  /// \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
  ///
  struct no_same_level_neighbors_impl_ {
    constexpr SInd operator()(const SInd m) const noexcept {
      if (nd < m || m < 0) { return 0; }
      return math::ct::ipow(2, nd - m) * math::ct::binomial_coefficient(nd, m);
    }
  };
  using no_same_level_neighbors_ = Foldable<no_same_level_neighbors_impl_>;

  template <SInd m>
  static constexpr SInd no_neighbors_(across<m>, at_same_level_t) noexcept {
    return no_same_level_neighbors_{}(across<m>{});
  }

  static constexpr SInd no_neighbors_(across_all_t, at_same_level_t) noexcept {
    return integral::fold(across_all_range{}, 0, no_same_level_neighbors_{});
  }

  struct max_no_neighbors_impl_ {
    constexpr SInd operator()(const SInd m) const noexcept {
      if (m < 0) { return 0; }
      using math::ct::ipow;
      using math::ct::factorial;
      const auto maxNoNeighborsPerFace = math::ct::ipow(2, m);
      const auto maxNoFaces = no_same_level_neighbors_{}(m);
      return maxNoNeighborsPerFace * maxNoFaces;
    }
  };
  using max_no_neighbors_ = Foldable<max_no_neighbors_impl_>;

  /// \name Across levels
  ///@{

  template <SInd m>
  static constexpr SInd no_neighbors_(across<m>, all_neighbors_t) noexcept {
    return max_no_neighbors_{}(across<m>{});
  }

  static constexpr SInd no_neighbors_(across_all_t, all_neighbors_t) noexcept {
    return integral::fold(across_all_range{}, 0, max_no_neighbors_{});
  }

  ///@}  // Across levels

 public:
  /// \f$\#\f$ of neighbors per node within the set \p NeighborSet and
  /// across \p Across
  ///
  /// \tparam Across e.g. across_faces, across_edges, across_corners,
  ///         across_all, ...
  /// \tparam NeighborSet e.g. at_same_level, all_neighbors.
  ///
  /// \complexity O(1)
  /// \returns number of neighbors
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  static constexpr SInd no_neighbors(Across = Across{},
                                     NeighborSet = NeighborSet{}) noexcept {
    const constexpr SInd n = no_neighbors_(Across{}, NeighborSet{});  // force
    return n;
  }

 private:
  /// Asserts that a given neighbor index is in range [0, no_neighbors)
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  static inline void assert_neighbor_idx(const neighbor_pos nIdx,
                                         const String at,
                                         Across = Across{},
                                         NeighborSet = NeighborSet{}) noexcept {
    ASSERT_AT(nIdx() < no_neighbors(Across{}, NeighborSet{}),
              "Neighbor index " << nIdx << " is not in range [0, "
              << no_neighbors(Across{}, NeighborSet{})
              << ")!",
              at);
  }


  ///@}  // Maximum number of neighbors

  /// \name Opposite neighbor stencils
  ///@{
 private:
  static constexpr neighbor_pos opposite_neighbor_position_(
   const neighbor_pos pos, across_faces_t, at_same_level_t) noexcept {
    assert_neighbor_idx(pos, AT_, across_faces, at_same_level);
    // the following is: pos % 2 == 0? pos + 1 : pos - 1
    // but branchless, and without comparisons
    const bool f = pos() % 2;
    return neighbor_pos{(!f) * (pos() + 1) + f * (pos() - 1)};
  }
  static constexpr neighbor_pos opposite_neighbor_position_(
   const neighbor_pos pos, across_edges_t, at_same_level_t) noexcept {
    assert_neighbor_idx(pos,AT_, across_edges, at_same_level);
    const constexpr std::array<typename neighbor_pos::value_type, 12> stencil
     = {{3, 2, 1, 0, 9, 8, 11, 10, 5, 4, 7, 6}};
    return neighbor_pos{stencil[pos()]};
  }
  static constexpr neighbor_pos opposite_neighbor_position_(
   const neighbor_pos pos, across_corners_t, at_same_level_t) noexcept {
    assert_neighbor_idx(pos,AT_, across_corners, at_same_level);
    const constexpr std::array<typename neighbor_pos::value_type, 8> stencil
     = {{7, 6, 5, 4, 3, 2, 1, 0}};
    return neighbor_pos{stencil[pos()]};
  }

  static constexpr neighbor_pos opposite_neighbor_position_(
   const neighbor_pos pos, across_all_t, at_same_level_t) noexcept {
    assert_neighbor_idx(pos,AT_, across_all, at_same_level);
    neighbor_pos pos_ = pos;
    neighbor_pos oppositePos{0};
    bool found = false;
    integral::for_each(neighbor_types{}, [&](const auto t) {
      if (!found) {
        const auto noNghbrs = no_neighbors(decltype(t){}, at_same_level);
        if (pos_() < noNghbrs) {
          found = true;
          oppositePos
           += opposite_neighbor_position(pos_, decltype(t){}, at_same_level);
        } else {
          pos_() -= noNghbrs;
          oppositePos() += noNghbrs;
        }
      }
    });
    return oppositePos;
  }

 public:
  /// Opposite neighbor position of \p pos across \p Across.
  ///
  /// I.e. the neighbor with position \p pos of a given nIdx, has nIdx as
  /// neighbor in its \p oppositePos.
  ///
  /// \param [in] pos position whose opposite is sought
  /// \tparam Across e.g., across_faces, across_edges, across_corners,...
  /// \tparam NeighborSet e.g. at_same_level, all_neighbors.
  ///
  /// \complexity O(1)
  /// \returns A position
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  static constexpr neighbor_pos opposite_neighbor_position(
   const neighbor_pos pos, Across = Across{},
   NeighborSet = NeighborSet{}) noexcept {
    return opposite_neighbor_position_(pos, Across{}, NeighborSet{});
  }

  ///@}  // Opposite neighbor stencils

 private:
  /// Position in parent of a sibling in direction "nghbrPos"
  /// from a sibling in position "childPos"
  ///
  /// \param [in] childPos position in parent of a node id
  /// \param [in] nghbrPos neighbor position with respect to nodePos
  ///
  /// \returns position in common parent of a sibling located at nghbrPos w.r.t.
  /// nodePos if it exist
  ///
  /// \todo remove the stencil
  static constexpr child_pos rel_sibling_position(
   const child_pos childPos, const neighbor_pos nghbrPos) noexcept {
    assert_child_idx(childPos, AT_);
    assert_neighbor_idx(nghbrPos, AT_);
    const constexpr typename child_pos::value_type i = invalid;
    const constexpr std::array<std::array<typename child_pos::value_type, 6>, 8>
     stencil = {{                       // << nghbr position
                 {{i, 1, i, 2, i, 4}},  // child 0
                 {{0, i, i, 3, i, 5}},  // child 1
                 {{i, 3, 0, i, i, 6}},  // child 2
                 {{2, i, 1, i, i, 7}},  // child 3
                 {{i, 5, i, 6, 0, i}},  // child 4
                 {{4, i, i, 7, 1, i}},  // child 5
                 {{i, 7, 4, i, 2, i}},  // child 6
                 {{6, i, 5, i, 3, i}}   // child 7
     }};
    return child_pos{stencil[childPos()][nghbrPos()]};
  }

  /// Asserts that a given child pos index is in range:
  static inline void assert_child_idx(const child_pos cIdx, const String at) noexcept {
    ASSERT_AT(cIdx() < no_children(), "Child index " << cIdx
              << " is not in range [0, "
              << no_children() << ")!",
              at);
  }


  /// \name Neighbor paths
  ///@{
 private:
  static constexpr std::array<neighbor_pos, 2> neighbor_path_(const neighbor_pos pos,
                                                          across_edges_t) {
    // const constexpr std::array<std::array<typename neighbor_pos::value_type, 2>,
    // 12>
    // stencil = {{{{0, 2}},  // pos 0
    //             {{1, 2}},  // pos 1
    //             {{0, 3}},  // pos 2
    //             {{1, 3}},  // pos 3
    //             {{0, 4}},  // pos 4
    //             {{1, 4}},  // pos 5
    //             {{2, 4}},  // pos 6
    //             {{3, 4}},  // pos 7
    //             {{0, 5}},  // pos 8
    //             {{1, 5}},  // pos 9
    //             {{2, 5}},  // pos 10
    //             {{3, 5}}   // pos 11
    // }};
    // return {{neighbor_pos{stencil[pos()][0]}, neighbor_pos{stencil[pos()][1]}}};
    if (pos() < 4) {
      return {{neighbor_pos{pos() % 2}, neighbor_pos{(pos() / 2) % 2 + 2}}};
    } else if (nd > 2) {
      return {{neighbor_pos{(pos() - 4) % 4}, neighbor_pos{((pos() - 4) / 4) % 2 + 4}}};
    } else { TERMINATE("this can't happen"); }
  }

  static constexpr std::array<neighbor_pos, 3> neighbor_path_(const neighbor_pos pos,
                                                          across_corners_t) {
    // const constexpr std::array<std::array<typename neighbor_pos::value_type, 3>,
    // 8>
    // stencil = {{{{0, 2, 4}},  // pos 0
    //             {{1, 2, 4}},  // pos 1
    //             {{0, 3, 4}},  // pos 2
    //             {{1, 3, 4}},  // pos 3
    //             {{0, 2, 5}},  // pos 4
    //             {{1, 2, 5}},  // pos 5
    //             {{0, 3, 5}},  // pos 6
    //             {{1, 3, 5}}   // pos 7
    // }};
    // return {{neighbor_pos{stencil[pos()][0]},
    //          neighbor_pos{stencil[pos()][1]},
    //          neighbor_pos{stencil[pos()][2]}}};
    return {{neighbor_pos{pos() % 2},              // 0 -> pos % 2
             neighbor_pos{(pos() / 2) % 2 + 2},    // 1 -> ((pos / 2) % 2) + 2
             neighbor_pos{(pos() / 4) % 2 + 4}}};  // 2 -> ((i / 4) % 2) + 4
  }

 public:
  /// Path of direct neighbor traversal to arrive at the neighbor located
  /// at \p pos across \p Across.
  ///
  /// \param [in] position of the neighbor sought across \p Across
  /// \tparam Across either across_edges or across_corners
  ///
  /// \complexity O(1)
  /// \returns a range of positions representing the path
  template <class Across>
  auto neighbor_path(const neighbor_pos position, Across) const
   RETURNS(neighbor_path_(position, Across{}));

  ///@}  // Neighbor paths

  ///@}  // Encoded spatial information

  //////////////////////////////////////////////////////////////////////////////

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// \name Data and memory layout
  ///@{

  /// \name Sibling group bounds
  ///
  /// Each node can be divided isotropically into no_siblings/no_children nodes.
  /// This groups of nodes sharing the same parent are denoted sibling groups.
  /// They are considered here a fundamental unit of memory.
  ///
  /// The following variables describe the available memory as a function of the
  /// number of sibling groups

  /// Max number of sibling groups that can be allocated
  const sibling_group_idx maxNoSiblingGroups_;
  /// Number of sibling groups in use
  sibling_group_idx noActiveSiblingGroups_;
  /// All sibling groups > onePastLastSiblingGroupInUse_ are unused
  sibling_group_idx onePastLastSiblingGroupInUse_;

  /// Number of sibling groups required to store \p no_nodes
  ///
  /// It is computed as follows
  /// floor((n + $of children - 1) / #of children)
  static constexpr auto no_sibling_groups(const Ind no_nodes) noexcept -> sibling_group_idx
  { return sibling_group_idx {
      std::floor((no_nodes + no_siblings() - 1.) / no_siblings())
    };
  }

  /// Max number of allocatable sibling groups
  inline auto sibling_group_capacity() const noexcept -> sibling_group_idx { return maxNoSiblingGroups_; }

  /// Number of sibling groups in use
  inline auto no_active_sibling_groups () const noexcept -> sibling_group_idx { return noActiveSiblingGroups_; }
  /// Number of sibling groups in use (writable reference)
  inline auto no_active_sibling_groups_()       noexcept -> sibling_group_idx& { return noActiveSiblingGroups_; }

 public:
  /// Is the hyperoctree empty?
  inline auto empty() const noexcept -> bool {
    return HOM3_UNLIKELY(no_active_sibling_groups()() == 0);
  }

 private:
  /// First sibling group in use
  static constexpr auto sibling_group_begin() noexcept -> sibling_group_idx { return sibling_group_idx{0}; }
  /// One past the last sibling group in use (all sibling groups in range
  /// [_, sibling_group_capacity) are unused)
  inline auto sibling_group_end() const noexcept -> sibling_group_idx { return onePastLastSiblingGroupInUse_; }
  /// One past the last sibling group in use (writable reference)
  inline auto sibling_group_end_() noexcept -> sibling_group_idx& { return onePastLastSiblingGroupInUse_; }

  /// \name Sibling group assertions
  ///@{

  /// Asserts that a sibling group has a valid Idx
  static inline auto assert_valid(const sibling_group_idx sgIdx, const String at)
      noexcept -> void {
    ASSERT_AT(is_valid(sgIdx), "Non-existent sibling group.", at);
  }

  /// Asserts that a sibling group is within the sibling group capacity range:
  /// [0, maxNoSiblingGroups_)
  inline auto assert_within_capacity(const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    assert_valid(sgIdx, at);
    ASSERT_AT(sgIdx >= sibling_group_begin() && sgIdx < sibling_group_capacity(),
              "sgIdx " << sgIdx << " is not within the sibling group capacity range ["
              << sibling_group_begin() << ", " << sibling_group_capacity() << ").", at);
  }

  /// Asserts that a sibling group is within the sibling group active range:
  /// [sibling_group_begin, sibling_group_end)
  inline auto assert_within_active(const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    assert_valid(sgIdx, at);
    ASSERT_AT(sgIdx >= sibling_group_begin() && sgIdx < sibling_group_end(),
              "sgIdx " << sgIdx << " is not within the sibling group active range ["
              << sibling_group_begin() << ", " << sibling_group_end() << ").", at);
  }

  ///@}  // Sibling group assertions

  ///@}  // Sibling group bounds

 public:
  /// \name Node bounds
  ///@{

  /// Max number of allocatable nodes
  auto node_capacity() const noexcept -> node_idx {
    return node_idx{1 + (sibling_group_capacity()() - 1) * no_siblings()};
    //             ^since if sibling_group_capacity == 1 => 1 node (root node)
  }
  /// Number of nodes in use
  auto no_nodes() const noexcept -> node_idx {
    return node_idx{1 + (no_active_sibling_groups()() - 1) * no_siblings()};
  }
  /// nIdx of the first node in the tree
  inline auto node_begin() const noexcept -> node_idx { return node_idx{0}; }
  /// nIdx of one past the last node _in use_ in the tree
  inline auto node_end() const noexcept -> node_idx {
    return node_idx{1 + (sibling_group_end()() - 1) * no_siblings()};
  }

 private:
  /// \name Node assertions
  ///@{

  /// Asserts that a node has a valid Idx
  static inline auto assert_valid(const node_idx nIdx, const String at)
   noexcept -> void {
    ASSERT_AT(is_valid(nIdx), "Non-existent node.", at);
  }

  /// Asserts that a node is within the node capacity range: [0, capacity)
  inline auto assert_within_capacity(const node_idx nIdx, const String at) const noexcept -> void {
    assert_valid(nIdx, at);
    ASSERT_AT(nIdx >= node_idx{0} && nIdx < node_capacity(),
              "nIdx " << nIdx << " is not within the node capacity range ["
              << node_begin() << ", " << node_capacity() << ").",
              at);
  }

  /// Asserts that node \p nIdx is within the active node range:
  /// [node_begin, node_end)
  inline auto assert_within_active(const node_idx nIdx, const String at) const noexcept -> void {
    assert_valid(nIdx, at);
    ASSERT_AT(nIdx >= node_begin() && nIdx < node_end(),
              "nIdx " << nIdx << " is not within the node active range ["
              << node_begin() << ", " << node_end() << ").", at);
  }

  ///@}  // Node assertions

  ///@}  // Node bounds

 private:
  /// \name Sibling group / node mapping
  ///@{

  /// Returns the sibling grou pidx of the node \p nIdx
  constexpr auto sibling_group(const node_idx nIdx) const noexcept -> sibling_group_idx {
    assert_within_capacity(nIdx, AT_);
    return no_sibling_groups(nIdx());
  }

  /// First node in a sibling group
  auto begin(const sibling_group_idx sgIdx) const noexcept -> node_idx {
    assert_within_capacity(sgIdx, AT_);
    return HOM3_UNLIKELY(sgIdx() == 0)? node_idx{0}
    : node_idx{1 + no_children() * (sgIdx() - 1)};
  }

  /// One past the last node in a sibling group
  auto end(const sibling_group_idx sgIdx) const noexcept -> node_idx {
    assert_within_capacity(sgIdx, AT_);
    return HOM3_UNLIKELY(sgIdx() == 0)? node_idx{1} : begin(sgIdx) + node_idx{no_children()};
  }

  ///@}  // Sibling group / node mapping

 private:
  /// \name Graph connectivity (edge arrays)
  /// @{

  /// Container for edges between sibling groups and their parent nodes:
  /// sibling_group_idx -> node_idx
  using parent_edge_vector = vector<node_idx, sibling_group_idx>;
  /// Edges between sibling groups and their parent nodes
  parent_edge_vector sibling_group_to_parent_edges_;
  /// Parent of sibling group \p sgIdx
  inline auto parent(const sibling_group_idx sgIdx) const noexcept -> node_idx {
    assert_within_capacity(sgIdx, AT_);
    return sibling_group_to_parent_edges_(sgIdx);
  }
  /// Parent of sibling group \p sgIdx
  inline auto parent_(const sibling_group_idx sgIdx) noexcept -> node_idx& {
    assert_within_capacity(sgIdx, AT_);
    return sibling_group_to_parent_edges_(sgIdx);
  }
 public:
  /// Parent of node \p nIdx
  inline auto parent(const node_idx nIdx) const noexcept -> node_idx {
    assert_within_capacity(nIdx, AT_);
    /// \todo assert in use?
    return parent(sibling_group(nIdx));
  }

 private:
  /// Container for edges between nodes and their children (sibling groups):
  /// node_idx -> sibling_group_idx
  using child_edge_vector = vector<sibling_group_idx, node_idx>;
  /// Edges between nodes and their children (sibling groups)
  child_edge_vector parent_to_children_group_edges_;
  /// Children group of node \p nIdx
  inline auto children_group(const node_idx nIdx) const noexcept -> sibling_group_idx {
    assert_within_capacity(nIdx, AT_);
    return parent_to_children_group_edges_(nIdx);
  }
  /// Children group of node \p nIdx (writable reference)
  inline auto children_group_(const node_idx nIdx) noexcept -> sibling_group_idx& {
    assert_within_capacity(nIdx, AT_);
    return parent_to_children_group_edges_(nIdx);
  }

 public:
  /// Child of \p nIdx at position \p pos
  inline auto child(const node_idx nIdx, const child_pos pos) const noexcept -> node_idx {
    assert_within_capacity(nIdx, AT_);
    assert_child_idx(pos, AT_);
    /// \todo assert in use?
    return is_valid(children_group(nIdx))
           ? begin(children_group(nIdx)) + node_idx{pos()}
           : invalid;
  }

  ///@}  // Graph connectivity (edge arrays)

 private:
  /// \name Unused memory (keep track of free memory)
  ///@{

  /// First free sibling group in range [sibling_group_begin, sibling_group_end)
  sibling_group_idx firstFreeSiblingGroup_;

  /// Container for tracking which sibling groups are in use: sibling_group_idx -> Bool
  using is_in_use_flags = bit_vector<sibling_group_idx>;
  /// Flags sibling groups as in use (true) or free (false)
  is_in_use_flags is_in_use_flags_;
    /// Is sibling group \p sgIdx in use? (writable reference)
  auto is_in_use_(const sibling_group_idx sgIdx) noexcept
  -> decltype(is_in_use_flags_(sgIdx)) {
    assert_within_capacity(sgIdx, AT_);
    return is_in_use_flags_(sgIdx);
  }
  /// Is sibling group \p sgIdx in use?
  auto is_in_use(const sibling_group_idx sgIdx) const noexcept -> bool {
    assert_within_capacity(sgIdx, AT_);
    return is_in_use_flags_(sgIdx);
  }
  /// Is sibling group \p free
  auto is_free(const sibling_group_idx sgIdx) const noexcept -> bool {
    assert_within_capacity(sgIdx, AT_);
    return !is_in_use(sgIdx);
  }
 public:
  /// Is node \p nIdx in use?
  auto is_in_use(const node_idx nIdx) const noexcept -> bool {
    assert_within_capacity(nIdx, AT_);
    return is_in_use(sibling_group(nIdx));
  }
  /// Is node \p nIdx free?
  auto is_free(const node_idx nIdx) const noexcept -> bool {
    assert_within_capacity(nIdx, AT_);
    return is_free(sibling_group(nIdx));
  }

 private:
  auto first_free_sibling_group() const noexcept -> sibling_group_idx { return firstFreeSiblingGroup_; }
  auto first_free_sibling_group_() noexcept -> sibling_group_idx& { return firstFreeSiblingGroup_; }
  /// Finds first free sibling group in range [start, sibling_group_capacity)
  auto first_free_sibling_group(sibling_group_idx start)
  const noexcept -> sibling_group_idx {
    for (const auto e = sibling_group_end(); start < e && is_in_use(start);
         ++start) {}
    return start;
  }

  /// \name Assertions
  ///@{
  inline auto assert_in_use(const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    assert_within_active(sgIdx, at);
    ASSERT(is_in_use(sgIdx), "Sibling group: " << sgIdx << " is not in use!");
  }

  inline auto assert_free(const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    assert_within_capacity(sgIdx, at);
    ASSERT(!is_in_use(sgIdx), "Sibling group: " << sgIdx << " is not free!");
  }

  inline auto assert_in_use(const node_idx nIdx, const String at) const noexcept -> void {
    assert_within_active(nIdx, at);
    ASSERT(is_in_use(nIdx), "Node: " << nIdx << " is not in use!");
  }

  inline auto assert_free(const node_idx nIdx, const String at) const noexcept -> void {
    assert_within_capacity(nIdx, at);
    ASSERT(!is_in_use(nIdx), "Node: " << nIdx << " is not free!");
  }

  inline auto assert_sibling_group_invariants
  (const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    ASSERT_AT(is_in_use(sgIdx) || (is_free(sgIdx) && is_reseted(sgIdx)),
              "sibling groups are either in use, or free and reseted", at);
    ASSERT_AT([&]() {
        if (is_free(sgIdx)) { return true; }

        /// Check the root sibling group:
        ASSERT_AT((sgIdx != sibling_group_idx{0})  // either not the root sg
               || (sgIdx == sibling_group_idx{0} // or the root sg:
                   && is_root(sgIdx)                         // root sibling group
                   && !is_valid(parent(sgIdx))               // has no parent
                   && ranges::distance(siblings(sgIdx)) == 1 // and has only one node
                   && begin(sgIdx) == node_idx{0}            // which is node 0
                   && is_root(begin(sgIdx))                  // which is the root node
                   && !is_valid(parent(begin(sgIdx))))      // and has no parent either
                   && is_in_use(sgIdx)                       // and are always in use
               && is_in_use(begin(sgIdx)),                    // (because non-free above)
               "root sibling group is sibling group 0, has only one node, "
               "that node is the root node, and neither the node nor the "
                  "sibling group have a parent", at);

        /// Check non-sibling group nodes:
        ASSERT_AT((sgIdx == sibling_group_idx{0})  // either root sg
               || (sgIdx != sibling_group_idx{0} // or non-root sg:
                   && is_in_use(sgIdx)                                                                       // which is in use (because non-free above)
                   && !is_root(sgIdx)                                                                     // which is not root
                   // Sibling group -> parent && parent -> sibling group:
                   && is_valid(parent(sgIdx))                                                             // and thus always has a valid parent
                   && children_group(parent(sgIdx)) == sgIdx                                              // which has the sibling group as children
                   && ranges::distance(siblings(sgIdx)) == no_siblings()                                  // always has no_siblings() siblings
                   && ranges::all_of(siblings(sgIdx), [&](auto i) { return is_valid(i); })                // which are all valid and
                   && ranges::all_of(siblings(sgIdx), [&](auto i) { return is_valid(parent(i)); })        // which all have a valid parent
                   && ranges::all_of(siblings(sgIdx), [&](auto i) { return parent(i) == parent(sgIdx); }) // which is the parent of the sibling group
                   // Sibling group -> children && children -> sibling group
                   && ranges::all_of(child_positions(), [&](auto p) {
                       auto s = begin(sgIdx) + node_idx{p};
                       ASSERT_AT((!is_valid(children_group(s)) // either have no children
                       && is_leaf(s))  // and thus are leaf nodes
                              // or they have children:
                              || (is_valid(children_group(s)) && !is_leaf(s) // and thus are not leafs
                           && parent(children_group(s)) == s // and the children have the sibling as parent
                                  ), "If they have children, "
                              "they aren't leaf siblings, and their children "
                              "have the sibling as parent. Otherwise they are "
                                 "leaf siblings.", at);
                       return true;
                     })), "non-root sg has valid parent, which has sg "
               "as child, there are no_siblings siblings, and all "
               "of them are valid, have a parent, and the parent "
                  "is the parent of the sg.", at);

        return true;
      }(), "assertions for sibling groups in use", at);
  }

  ///@}  // Assertions

  ///@}   /// Unused memory (keep track of free memory)

 private:
  /// \name Node data
  /// \todo
  ///@{

  using data = typename data_layout::data;
  data data_;

  ///@}

 private:
  /// \name Initialization
  ///@{

  /// Initializes the tree with a root node
  auto initialize_tree_with_root_node() noexcept -> void {
    if(!empty()) { TERMINATE("Tree is not empty!"); }
    allocate_siblings();
  }

  /// Activates the free sibling group \p sgIdx
  auto activate(const sibling_group_idx sgIdx) noexcept -> void {
    assert_within_capacity(sgIdx, AT_);
    assert_free(sgIdx, AT_);
    is_in_use_(sgIdx) = true;
    ++no_active_sibling_groups_();
    if (sgIdx == sibling_group_end()) {
      ++sibling_group_end_();
    }
    first_free_sibling_group_() = first_free_sibling_group(sgIdx);
  }

  /// Allocates memory for a sibling group
  ///
  /// \returns Sibling idx of the group (invalid if allocation fails)
  ///
  /// \pre Allocation succeeds _iff_ node_end() + no_children() <= capacity()
  /// \post _Iff_ allocation succeeds node_end() is incremented by no_children()
  /// \post _Iff_ allocation succeeds _and_ firstFreeSiblingGroup <
  /// no_sibling_groups(size())
  ///       the next free sibling group is found.
  ///
  /// \invariant Strong-guarantee: no observable effects if allocation fails
  ///
  auto allocate_siblings() noexcept -> sibling_group_idx {
    /// If allocation is not possible -> invalid sibling group
    if (HOM3_UNLIKELY(no_active_sibling_groups()() + 1 > sibling_group_capacity()())) {
      return sibling_group_idx{invalid};
    }
    /// Otherwise use the first free sibling group:
    const auto sg = first_free_sibling_group();
    assert_reseted(sg, AT_);
    assert_free(sg, AT_);
    activate(sg);
    assert_in_use(sg, AT_);
    return sg;
  }

  ///@}  // Initialization

  /// \name Deletion
  ///@{

  /// Deactivates the currently in use sibling group \p sgIdx
  auto deactivate(const sibling_group_idx sgIdx)  noexcept -> void {
    assert_within_capacity(sgIdx, AT_);
    assert_in_use(sgIdx, AT_);
    is_in_use_(sgIdx) = false;
    --no_active_sibling_groups_();
    if (sgIdx == sibling_group_end()) {
      --sibling_group_end_();
    }
    if (sgIdx < first_free_sibling_group()) {
      first_free_sibling_group_() = sgIdx;
    }
    assert_free(sgIdx, AT_);
  }

  /// Resets a sibling group (without deactivation)
  auto reset(const sibling_group_idx sgIdx) noexcept -> void {
    assert_within_capacity(sgIdx, AT_);
    for (auto&& nIdx : siblings(sgIdx)) { children_group_(nIdx) = invalid; }
    parent_(sgIdx) = invalid;
    assert_reseted(sgIdx, AT_);
  }

  /// Resets the tree
  auto reset() noexcept -> void {
    for (auto&& i : allocated_nodes()) { children_group_(i) = invalid; }
    for (auto&& i : allocated_sibling_groups()) { parent_(i) = invalid; }
    is_in_use_flags_.reset();
    no_active_sibling_groups_() = sibling_group_idx{0};
    sibling_group_end_() = sibling_group_idx{0};
    first_free_sibling_group_() = sibling_group_idx{0};
  }

  auto is_reseted(const sibling_group_idx sgIdx) const noexcept -> bool {
    assert_within_capacity(sgIdx, AT_);
    return !is_valid(parent(sgIdx))
        && ranges::all_of(siblings(sgIdx), [&](auto sIdx) {
            return !is_valid(children_group(sIdx));
    });
  }

  auto is_reseted(const node_idx nIdx) const noexcept -> bool {
    assert_within_capacity(nIdx, AT_);
    return !is_valid(parent(nIdx)) && !is_valid(children_group(nIdx));
  }

  auto assert_reseted(const sibling_group_idx sgIdx, const String at) const noexcept -> void {
    assert_within_capacity(sgIdx, at);
    ASSERT_AT(is_reseted(sgIdx), "sibling group: " << sgIdx << " is not reseted!", at);
  }

  /// Deallocates sibling group \p sgIdx (resets + deactivates)
  auto deallocate_siblings(const sibling_group_idx sgIdx) noexcept -> void {
    assert_within_capacity(sgIdx, AT_);
    reset(sgIdx);
    deactivate(sgIdx);
    assert_free(sgIdx, AT_);
    assert_reseted(sgIdx, AT_);
  }

  /// Swaps the location in memory of two groups of siblings \p sgL and \p sgR
  ///
  /// \invariant Maintains all tree invariants and graph.
  ///
  /// \complexity O(1)
  auto swap_siblings(const sibling_group_idx sgL, const sibling_group_idx sgR) -> void {
    using std::swap; using std::get;
    assert_within_capacity(sgL, AT_);
    ASSERT(sgL() != 0 && sgR() != 0, "root node is not swappable!");
    assert_within_capacity(sgR, AT_);
    ASSERT(sgL != sgR, "self swap is easily implementable!");
    assert_sibling_group_invariants(sgL, AT_);
    assert_sibling_group_invariants(sgR, AT_);

    /// 0) Break early optimizations: both not in use -> nothing to do
    if (is_free(sgL) && is_free(sgR)) { return; }

    /// 1) swap siblings -> children edges, and children -> sibling edges:
    auto update_cg_parent = [&](auto s) {
      const auto child_cg = children_group(s);
      if (is_valid(child_cg)) { parent_(child_cg) = s; }
    };
    for (auto s : ranges::zip(siblings(sgL), siblings(sgR))) {
      swap(children_group_(get<0>(s)), children_group_(get<1>(s)));
      update_cg_parent(get<0>(s));
      update_cg_parent(get<1>(s));
    }

    /// 2) swap parent -> sibling edges, and sibling -> parent edges:
    auto update_parent_sibling_e = [&](auto parent, auto sibling_g) {
      if (is_valid(parent)) {
        children_group_(parent) = sibling_g;
        parent_(sibling_g) = parent;
      } else {
        parent_(sibling_g) = invalid;
      }
    };
    {
      const auto pL = parent(sgL);
      const auto pR = parent(sgR);
      update_parent_sibling_e(pL, sgR);
      update_parent_sibling_e(pR, sgL);
    }

    /// 3) swap active flag
    {
      const bool in_use_l = is_in_use(sgL);
      is_in_use_(sgL) = is_in_use(sgR);
      is_in_use_(sgR) = in_use_l;
    }

    /// 4) update the firstFreeSiblingGroup_ flag:
    ///    - if one of the nodes is inactive:
    if((is_free(sgL) || is_free(sgR)) &&
       /// - if the flag is between the range:
       (std::min(sgL, sgR) <= first_free_sibling_group() &&
        first_free_sibling_group() <= std::max(sgL, sgR)) ) {
      first_free_sibling_group_() = first_free_sibling_group(std::min(sgL, sgR));
    }

    /// 5) update the one past last sibling group in use flag:
    if (is_free(sgL) || is_free(sgR)) {
      if (sgL == sibling_group_end() - sibling_group_idx{1}
          || sgR == sibling_group_end() - sibling_group_idx{1}) {
        --sibling_group_end_();
      }
    }

    assert_sibling_group_invariants(sgL, AT_);
    assert_sibling_group_invariants(sgR, AT_);
  }

  ///@}  // Data and memory layout
  //////////////////////////////////////////////////////////////////////////////

 public:
  /// Constructs a grid container that can maximally hold \p maxNoNodes
  ///
  /// \param [in] input property list containing the key maxNoNodes
  ///
  /// \property maxNoNodes allowed maximum number of nodes in the tree
  explicit hyperoctree(const io::properties input)
   : maxNoSiblingGroups_{no_sibling_groups(io::read<Ind>(input, "maxNoNodes"))}
      , sibling_group_to_parent_edges_{sibling_group_capacity()()}
      , parent_to_children_group_edges_{node_capacity()()}
      , is_in_use_flags_{sibling_group_capacity()()}
      , data_(data_layout::make(node_capacity()())) {
        reset();
     initialize_tree_with_root_node();
  }

  hyperoctree() = delete;
  hyperoctree(hyperoctree const&) = default;
  hyperoctree(hyperoctree&&) = default;
  hyperoctree& operator=(hyperoctree const&) = default;
  hyperoctree& operator=(hyperoctree&&) = default;

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// \name Non-modifying queries
  ///@{

   public:
  /// Number of leaf nodes in the tree
  ///
  /// \complexity O(N)
  inline auto no_leaf_nodes() const noexcept -> node_idx {
    return node_idx{ranges::distance(leaf_nodes())};
  }
  /// Is the node \p nIdx a root node?
  /// (right now there is only a single root node)
  inline auto is_root(const node_idx nIdx) const noexcept -> bool {
    assert_in_use(nIdx, AT_);
    ASSERT(nIdx != node_idx{0} || (nIdx == node_idx{0} && !is_valid(parent(nIdx))),
           "root node is nIdx = 0 and has no parent");
    return HOM3_UNLIKELY(nIdx == node_idx{0});
  }
  inline auto is_root(const sibling_group_idx sgIdx) const noexcept -> bool {
    assert_in_use(sgIdx, AT_);
    ASSERT(sgIdx != sibling_group_idx{0}
           || (sgIdx == sibling_group_idx{0} && !is_valid(parent(sgIdx))),
           "root sibling group is sgIdx = 0 and has no parent");
    return HOM3_UNLIKELY(sgIdx == sibling_group_idx{0});
  }
  /// Is node \p nIdx a leaf node?
  inline auto is_leaf(const node_idx nIdx) const noexcept -> bool {
    assert_in_use(nIdx, AT_);
    return HOM3_LIKELY(no_children(nIdx) == 0);  // most nodes are leafs
  }
  /// \f$\#\f$ of children of \p nIdx
  inline auto no_children(const node_idx nIdx) const noexcept -> SInd {
    assert_in_use(nIdx, AT_);
    return static_cast<SInd>(ranges::distance(children(nIdx)));
  }
  /// Does node \p nIdx have children?
  inline auto has_children(const node_idx nIdx) const noexcept -> bool {
    assert_in_use(nIdx, AT_);
    // most nodes (leafs) have no children:
    return HOM3_UNLIKELY(!is_leaf(nIdx));
  }
  /// Does node \p nIdx have _all_ children? I.e.
  /// is \f$\#\f$ of children of \p nIdx == \f$\#\f$ of children positions?
  inline auto has_all_children(const node_idx nIdx) const noexcept -> bool {
    assert_in_use(nIdx, AT_);
    // most nodes (leafs) have no children
    return HOM3_UNLIKELY(no_children(nIdx) == no_children());
  }
  /// Position in parent of node \p nIdx
  inline auto position_in_parent(const node_idx nIdx) const noexcept -> child_pos {
    assert_in_use(nIdx, AT_);
    ASSERT(!is_root(nIdx), "root cell has no parent!");
    assert_in_use(parent(nIdx), AT_);
    return child_pos{(nIdx() - 1) % no_children()};
  }

  /// \name Ranges
  ///@{

 private:
  /// \name Sibling group ranges
  ///@{

  /// Sibling nodes of sibling group \p sgIdx
  inline auto siblings(const sibling_group_idx sgIdx) const noexcept
      -> ranges::range<node_idx> {
    assert_within_capacity(sgIdx, AT_);
    return ranges::range<node_idx>(begin(sgIdx), end(sgIdx));
  }

  /// Range of all allocated sibling groups
  inline auto allocated_sibling_groups() const
   RETURNS(ranges::make<sibling_group_idx>(0, sibling_group_capacity()));

  /// Range of all sibling groups in use:
  inline auto sibling_groups() const
      RETURNS(ranges::make<sibling_group_idx>(0, sibling_group_end()));

  ///@}  // Sibling group ranges


 public:
  /// \name Node range filters
  ///@{

  /// Returns filtered range of existing node ids
  inline auto valid() const noexcept -> ranges::range_filter<node_idx> {
    return ranges::make_filter<std::function<bool(node_idx)>>(
     [&](const node_idx nIdx) { return is_valid(nIdx); });
  }
  /// Returns filtered range of active node ids (in use)
  inline auto in_use() const noexcept -> ranges::range_filter<node_idx> {
    return ranges::make_filter<std::function<bool(node_idx)>>(
     [&](const node_idx nIdx) { return is_in_use(nIdx); });
  }
  /// Returns filtered range of leaf node ids:
  ///
  /// \warning Apply only on ranges of active nodes
  inline auto leafs() const noexcept -> ranges::range_filter<node_idx> {
    return ranges::make_filter<std::function<bool(node_idx)>>(
     [&](const node_idx nIdx) { return is_leaf(nIdx); });
  }
  /// Returns filtered range of node ids at level "l"
  inline auto at_level(const SInd l) const
   noexcept -> ranges::range_filter<node_idx> {
    return ranges::make_filter<std::function<bool(node_idx)>>(
     [&, l](const node_idx nIdx) { return level(nIdx) == l; });
  }
  /// Maps child positions to child indices
  inline auto child_position_to_idx(const node_idx nIdx) const
   noexcept -> ranges::range_transformer<child_pos, node_idx> {
    assert_in_use(nIdx, AT_);
    return ranges::make_transform<std::function<node_idx(child_pos)>>(
     [&, nIdx](const child_pos pos) {
       assert_child_idx(pos, AT_);
       const auto first_child_idx = child(nIdx, child_pos{0});
       // Note 1: if node is leaf, child_(nIdx) is
       // invalid == numeric_limits<Ind>::max -> adding stuff to it makes it
       // either wrap around or is UB!
       //
       // Note 2: this filter is typically used to iterate over nodes _that_
       // do have children.
       return HOM3_LIKELY(is_valid(first_child_idx))
               ? first_child_idx + node_idx{pos()}
               : invalid;
     });
  }
  /// Maps neighbor positions to neighbor indices
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  inline auto neighbor_position_to_idx(const node_idx nIdx, Across = Across{},
                                       NeighborSet = NeighborSet{}) const
   noexcept -> ranges::range_transformer<neighbor_pos, node_idx> {
    assert_in_use(nIdx, AT_);
    return ranges::make_transform<std::function<node_idx(neighbor_pos)>>(
     [&, nIdx](const neighbor_pos pos) {
       assert_neighbor_idx(pos, AT_, Across{}, NeighborSet{});
       return neighbor(nIdx, pos, Across{}, NeighborSet{});
     });
  }

  ///@}  // Node range filters

 public:
  /// \name Node ranges
  ///@{
 private:
  /// Range of _all_ nodes (including inactive nodes).
  inline auto allocated_nodes() const
   RETURNS(ranges::range<node_idx>(node_begin(), node_capacity()));
 public:
  /// Range of _all_ nodes (including inactive nodes if there are any).
  inline auto nodes() const
   RETURNS(ranges::range<node_idx>(node_begin(), node_end()));
  /// Range of active leaf nodes.
  inline auto leaf_nodes() const RETURNS(nodes() | in_use() | leafs());
  /// Range of child positions.
  static inline auto child_positions()
  RETURNS(ranges::range<child_pos>(child_pos{0}, child_pos{no_children()}));
  /// Range of _all_ children of node \p nIdx (including inactive
  /// children).
  inline auto all_children(const node_idx nIdx) const
  RETURNS(child_positions() | child_position_to_idx(nIdx));
  /// Range of active children of node \p nIdx.
  inline auto children(const node_idx nIdx) const
  RETURNS(all_children(nIdx) | valid() | in_use());

  /// Range of neighbor positions across \p Across within the set \p
  /// NeighborSet.
  ///
  /// \tparam Across e.g. across_faces, across_edges, across_corners,
  ///         across_all,...
  /// \tparam NeighborSet e.g. at_same_level, all_neighbors,...
  ///
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  inline auto neighbor_positions(Across = Across{},
                                 NeighborSet = NeighborSet{}) const
   RETURNS(ranges::range<neighbor_pos>(
    neighbor_pos{0}, neighbor_pos{no_neighbors(Across{}, NeighborSet{})}));

 private:
  /// Sibling group range in DFS order:
  struct sibling_group_range {
    explicit sibling_group_range(this_type const& t, const sibling_group_idx sgIdx,
                                 const child_pos next_child_pos = child_pos{0})
        : tree(t), sgIdx_(sgIdx), endIdx_(sgIdx), next_child_pos_(next_child_pos) {}
    sibling_group_range(sibling_group_range const&) = default;
    sibling_group_range(sibling_group_range&&) = default;
    sibling_group_range& operator=(sibling_group_range const&) = default;
    sibling_group_range& operator=(sibling_group_range&&) = default;

   private:
    this_type const& tree;
    sibling_group_idx sgIdx_;
    sibling_group_idx endIdx_;
    child_pos next_child_pos_;

    /// Moves the iterator to the parent of the current sibling group and marks
    /// it (the current one) as visited
    auto move_to_parent() noexcept -> void {
      const auto pIdx = tree.parent(sgIdx_);
      if (is_valid(pIdx)) {
        if(pIdx == node_idx{0}) {
          sgIdx_ = sibling_group_idx{0};
        } else {
          next_child_pos_ = ++(tree.position_in_parent(tree.parent(sgIdx_)));
          sgIdx_ = tree.sibling_group(tree.parent(sgIdx_));
        }
      }
    }

   public:
    auto next() noexcept -> bool {
      /// At root node:
      if (sgIdx_() == 0) {
        if(next_child_pos_() == 0) { // next is first child (-> hasn't been visited yet)
          if (tree.is_leaf(node_idx{0})) {  // if root node is leaf, we are done
            return false;
          } else {  // otherwise next sibling group is the root node children
            sgIdx_ = tree.children_group(node_idx{0});
            return true;
          }
        } else { // next is not first child, we must be coming back, so we are done
          ASSERT(endIdx_ == sgIdx_, "??");
          return false;
        }
      }
      /// Find next valid children group starting at next_child_pos:
      node_idx next_child_idx = invalid;
      while(next_child_pos_ != child_pos{no_children()}) {
        auto pIdx =tree.parent(sgIdx_);
        auto cIdx = tree.child(pIdx, next_child_pos_);
        next_child_idx = tree.child(cIdx, child_pos{0});
        if(is_valid(next_child_idx)) {
          break;
        }
        ++next_child_pos_;
      }

      /// If not found:
      if (next_child_pos_== child_pos{no_children()}) {
        /// Are we done?
        if (sgIdx_ == endIdx_) {
          return false;
        } else {  /// move to parent and recurse:
          move_to_parent();
          return next();
        }
      } else {  /// Otherwise move to the next sibling group:
        sgIdx_ = tree.sibling_group(next_child_idx);
        next_child_pos_= child_pos{0};
        return true;
      }
    }

    auto operator()() const noexcept -> sibling_group_idx { return sgIdx_; }
  };

 public:
  /// DFS sibling group range
  inline auto dfs_siblings(const node_idx nIdx = node_idx{0}) const -> sibling_group_range {
    auto sgIdx = sibling_group(nIdx);
    return sibling_group_range{*this, sgIdx};
  }

  ///@}  // Node ranges
  ///@}  // Ranges
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Algorithms
  ///@{
 public:
  /// Computes the level of the node \p nIdx
  /// \complexity
  ///   - O(\f$\#\f$l): linear in the number of levels,
  ///   - O(log(\f$\#\f$size)): logarithmic in the number of nodes
  inline auto level(node_idx nIdx) const noexcept -> SInd {
    assert_in_use(nIdx, AT_);
    SInd level_ = 0;
    nIdx = parent(nIdx);
    // traverse the tree up to the root node:
    while (HOM3_LIKELY(is_valid(nIdx))) {  // most nodes are valid
      assert_in_use(nIdx, AT_);
      nIdx = parent(nIdx);
      ++level_;  // counting the levels
    }
    ASSERT(level_ < max_no_levels(), "error, level " + std::to_string(level_)
                                     + " exceeds max level "
                                     + std::to_string(max_no_levels()) + "!");
    return level_;
  }

  /// Returns the max level of the grid
  /// \complexity O(N)
  inline auto max_level() const noexcept -> SInd {
    using ranges::accumulate;
    return accumulate(leaf_nodes(), 0, [&](const SInd acc, const node_idx nIdx) {
      return std::max(acc, level(nIdx));
    });
  }

  /// Number of neighbors of node \p nIdx across \t Across in the set \t
  /// NeighborSet
  ///
  /// \param [in] nIdx node whose number of neighbors is to be computed
  /// \tparam Across e.g. across_faces, across_edges, across_corners,
  ///         across_all...
  /// \tparam NeighborSet e.g. at_same_level, all_neighors
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  inline auto no_neighbors(const node_idx nIdx, Across = Across{},
                           NeighborSet = NeighborSet{}) const noexcept -> SInd {
    using ranges::distance;
    assert_in_use(nIdx, AT_);
    return distance(neighbors(nIdx, Across{}, NeighborSet{}, eager())
                    | valid());
  }

 private:
  /// Computes the same level neighbor of the node \p nIdx located at
  /// its relative position \p pos.
  ///
  /// \param [in] nIdx  node id of the node whose neighbor is to be determined.
  /// \param [in] nghbrPos position of the required neighbor w.r.t.
  ///             the node nIdx.
  ///
  /// \returns [Ind] node idx of the neighbor located at pos w.r.t. the node
  /// nIdx. If no neighbor is found it returns invalid<node_idx>().
  ///
  /// \complexity worst case is O(\f$\#\f$l) = 2 * \f$\#\f$l, i.e.
  /// need to traverse the tree up to the root and back down again.
  ///
  /// \requires a balanced tree that satisfies 2:1 rule.
  auto find_samelevel_neighbor(const node_idx nIdx, const neighbor_pos nghbrPos,
                                  across_faces_t) const noexcept -> node_idx {
    using namespace ranges;
    DBG("start find_samelevel_neighbor | nIdx: ", nIdx, " | nghbrPos ",
        nghbrPos);
    assert_in_use(nIdx, AT_);
    assert_neighbor_idx(nghbrPos, AT_);

    /// The root node has no neighbors:
    if (HOM3_UNLIKELY(is_root(nIdx))) {
      DBG("exit find_samelevel_neighbor: nIdx ", nIdx,
          " is a root node | no nghbr found in pos: ", nghbrPos);
      return node_idx{invalid};
    }

    // Vector of traversed positions
    auto traversedPositions = stack::vector<child_pos, max_no_levels()>{};

    /// Travel up the tree until a node i is found, s.t. the previously
    /// traversed node is a child of i in the position that is "opposite" to pos
    /// (opposite in the sense of opposite_neighbor_position(nghbrPos), see that
    /// function call for details). The traversed positions are saved.
    auto commonParentFound = false;
    auto currentNode = nIdx;
    DBG("starting up-traversal at node: ", nIdx);
    while (!commonParentFound) {
      DBGV((currentNode));
      const auto pIdx = parent(currentNode);
      const auto posInParent = position_in_parent(currentNode);
      DBGV((parent(currentNode))(posInParent)(
       rel_sibling_position(posInParent, nghbrPos)));

      /// If the parent has a sibling in direction "nghbrPos" we have found
      /// a common parent and are done
      if (is_valid(rel_sibling_position(posInParent, nghbrPos))) {
        DBG("common parent found -> up-traversal finished");
        commonParentFound = true;
        break;
      }

      if (HOM3_UNLIKELY(is_root(pIdx))) {
        /// If the parent is the root and there is no sibling in direction
        /// "nbghrPos", node has no neighbor and we are done
        DBG("exit find_samelevel_neighbor: ",
            "samelparent is root, and no sibling in direction found!",
            "-> nghbr not found for nIdx: ", nIdx, ", nghbrPos: ", nghbrPos);
        return node_idx{invalid};
      } else {
        /// Keep on going up the tree
        traversedPositions.emplace_back(posInParent);
        currentNode = pIdx;
      }
    }
    ASSERT(commonParentFound, "No common parent has been found!");

    /// This is the opposite node at the common parent:
    currentNode
     = child(parent(currentNode),
             rel_sibling_position(position_in_parent(currentNode), nghbrPos));

    DBG("starting down traversal at node: ", currentNode);
    /// Traverse the tree back from the opposite node in opposite order to find
    /// the neighbor:
    for (const auto& traversedPosition : traversedPositions | reversed) {
      const auto nextchild_position = rel_sibling_position(
       traversedPosition, opposite_neighbor_position(nghbrPos));
      const auto nextIdx = child(currentNode, nextchild_position);
      DBGV((currentNode)(traversedPosition)(nextchild_position)(nextIdx));
      if (HOM3_LIKELY(is_valid(nextIdx))) {  // for a balanced tree: likely
        currentNode = nextIdx;
      } else {
        DBG("exit find_samelevel_neighbor: ", "nextIdx: ", nextIdx,
            " does not exist!", " -> nghbr not found for nIdx ", nIdx,
            ", nghbrPos: ", nghbrPos);
        return node_idx{invalid};
      }
    }

    DBG("nghbr found for nIdx: ", nIdx, ", nghbrPos: ", nghbrPos, "!",
        " -> nghbrIdx: ", currentNode);

    // Current node must be an active, valid neighbor
    assert_in_use(currentNode, AT_);
    ASSERT(level(nIdx) == level(currentNode),
           "Neighbors are not at the same level!");
    DBG("exit find_samelevel_neighbor");
    return currentNode;
  }

  /// Given a \t Path of \p neighborPositions, returns the same level
  /// neighbor located at the end of the path starting at node \p nIdx
  ///
  /// \param [in] nIdx node index at the beginning of the path
  /// \param [in] neighborPositions path of neighbor positions
  ///
  /// \complexity O(M * 2 * l) where M is the number of positions in the path
  template <class neighbor_positionsPath, class Across = across_faces_t>
  inline auto find_samelevel_neighbor(
   const node_idx nIdx, const neighbor_positionsPath neighborPositions,
   Across = Across{}) const noexcept -> node_idx {
    using ranges::next_permutation;
    DBG("start find_samelevel_neighbor path | nIdx: ", nIdx,
        " positions in path: ");
    ASSERT([&]() {
             for (auto pos : neighborPositions) { DBG("pos: ", pos(), " | "); }
             return true;
           }(),
           "print neighbor positions");

    assert_in_use(nIdx, AT_);
    node_idx result = invalid;
    auto positions = neighborPositions;
    do {
      result = nIdx;
      for (auto pos : positions) {
        assert_neighbor_idx(pos, AT_, Across{}, at_same_level);
        result = find_samelevel_neighbor(result, pos, Across{});
        if (HOM3_UNLIKELY(!is_valid(result))) {  // most nodes have all neighbrs
          break;
        }
      }
      if (HOM3_LIKELY(is_valid(result))) {  // most nodes have all neighbors
        break;
      }
      // in 2D: 50%/50%, in 3D: 66%/33% for likely
    } while (HOM3_LIKELY(next_permutation(positions)));

    ASSERT(
     [&]() {
       positions = neighborPositions;
       node_idx result2 = nIdx;
       do {
         for (auto pos : positions) {
           result2 = find_samelevel_neighbor(result2, pos, Across{});
           if (!is_valid(result2)) { break; }
         }
         if (is_valid(result2)) {
           ASSERT(result2 == result,
                  "neighbor found: permutation doesn't deliver same neighbor!");
         }
         if (!is_valid(result)) {
           ASSERT(!is_valid(result2),
                  "neighbor not found: no permutation can find a neighbor!");
         }
         result2 = nIdx;
       } while (ranges::next_permutation(positions));
       return true;
     }(),
     "all permutations delivering a neighbor should deliver same neighbor!");

    DBG("exit find_samelevel_neighbor path | nIdx: ", nIdx);
    return result;
  }

  /// Uses neighbor_path for edges to compute the edges neighbors via
  /// face neighbor traversals
  auto find_samelevel_neighbor(const node_idx nIdx, const neighbor_pos pos,
                                  across_edges_t) const noexcept -> node_idx {
    assert_in_use(nIdx, AT_);
    return find_samelevel_neighbor(nIdx, neighbor_path(pos, across_edges),
                                   across_faces);
  }

  /// Uses neighbor_path for corners to compute the corner neighbors via
  /// face neighbor traversals
  auto find_samelevel_neighbor(const node_idx nIdx, const neighbor_pos pos,
                                  across_corners_t) const noexcept -> node_idx {
    assert_in_use(nIdx, AT_);
    return find_samelevel_neighbor(nIdx, neighbor_path(pos, across_corners),
                                   across_faces);
  }

  /// Find same level neighbor of \p nIdx at position \p pos across \p
  /// Across  (overload for faces, edges, corners... not for across_all).
  template <class Across>
  auto find_neighbor(const node_idx nIdx, const neighbor_pos pos, Across,
                     at_same_level_t) const
   RETURNS(find_samelevel_neighbor(nIdx, pos, Across{}));

  /// Find same level neighbor of \p nIdx at position \p pos across all
  /// nd-m manifolds.
  ///
  /// \todo simplify the code (the found variable is ugly)
  auto find_neighbor(const node_idx nIdx, const neighbor_pos pos, across_all_t,
                        at_same_level_t) const -> node_idx {
    neighbor_pos pos_ = pos;
    assert_in_use(nIdx, AT_);
    assert_neighbor_idx(pos_, AT_, across_all, at_same_level);

    bool found = false;
    node_idx nghbr = invalid;
    integral::for_each(neighbor_types{}, [&](const auto t) {
      if (!found) {
        const auto noNghbrs = no_neighbors(decltype(t){}, at_same_level);
        if (pos_() < noNghbrs) {
          found = true;
          nghbr = find_neighbor(nIdx, pos_, decltype(t){}, at_same_level);
        } else { pos_() -= noNghbrs; }
      }
    });
    return nghbr;
  }

  /// Find neighbor across refinement levels
  /// \warning Not implemented yet
  template <class Across>
  auto find_neighbor(const node_idx, const neighbor_pos, Across,
                     all_neighbors_t) const noexcept {
    static_assert((std::remove_reference_t<Across>{}, false),
                  "neighbor search across levels not implemented!");
    TERMINATE("not implemented!");
  }

  /// Returns array containing the neighbors of the node \p nIdx across
  /// \p Across within the set \p NeighborSet.
  ///
  /// \param [in] nIdx node id of the node whose neighbors are to be computed.
  /// \tparam Across e.g., across_faces, across_edges, across_corners,
  ///                acros_all.
  /// \tparam NeighborSet e.g., at_same_level, all_neighbors
  ///
  /// \returns Array containing the neighbors of node \p nIdx across \p Across
  /// within the set \p NeighborSet.
  template <class Across, class NeighborSet>
  auto find_neighbors(const node_idx nIdx, Across, NeighborSet, eager) const noexcept
  -> neighbor_ids<no_neighbors(Across{}, NeighborSet{})> {
    assert_in_use(nIdx, AT_);
    neighbor_ids<no_neighbors(Across{}, NeighborSet{})> nodeNghbrs;
    for (const auto& pos : neighbor_positions(Across{}, NeighborSet{})) {
      nodeNghbrs(pos) = find_neighbor(nIdx, pos, Across{}, NeighborSet{});
    }
    return nodeNghbrs;
  }

  /// Returns lazy view of the neighbors of the node \p nIdx across \p
  /// Across within the set \p NeighborSet.
  ///
  /// \param [in] nIdx node id of the node whose neighbors are to be computed.
  /// \tparam Across e.g., across_faces, across_edges, across_corners,
  ///                acros_all.
  /// \tparam NeighborSet e.g., at_same_level, all_neighbors
  ///
  /// \returns Returns lazy view of the neighbors of node \p nIdx across \p
  /// Across within the set \p NeighborSet.
  template <class Across, class NeighborSet>
  auto find_neighbors(const node_idx nIdx, Across, NeighborSet, lazy) const
   RETURNS(neighbor_positions(Across{}, NeighborSet{})
           | neighbor_position_to_idx(nIdx, Across{}, NeighborSet{}));

 public:
  /// Returns the range of neighbors of node \p nIdx across \p Across in
  /// the set \p NeighborSet evaluated according to \p Evaluation.
  ///
  /// \param [in] nIdx node idx whose neighbor set is to be computed.
  /// \tparam Across e.g. across_faces, across_edges, across_corners,
  ///                across_all.
  /// \tparam NeighborSet e.g. at_same_level, all_neighbors.
  /// \tparam Evaluation eager/lazy
  ///
  /// \warning NeighborSet all_neighbors not implemented yet (this will
  ///          only compile for computing same_level_neighbors).
  ///
  /// \complexity TODO
  /// \return TODO
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t,
            class Evaluation = eager>
  auto neighbors(const node_idx nIdx, Across = Across{},
                 NeighborSet = NeighborSet{}, Evaluation = Evaluation()) const
   RETURNS(find_neighbors(nIdx, Across{}, NeighborSet{}, Evaluation{}));

  /// Returns the neighbor of node \p nIdx across \p Across within
  /// the set \p NeighborSet at position \p pos.
  ///
  /// \param [in] nIdx node idx whose neighbor set is to be computed.
  /// \param [in] pos neighbor position within the set.
  /// \tparam Across e.g. across_faces, across_edges, across_corners,
  ///                across_all.
  /// \tparam NeighborSet e.g. at_same_level, all_neighbors.
  ///
  /// \warning NeighborSet all_neighbors not implemented yet (this will
  ///          only compile for computing same_level_neighbors).
  ///
  /// \complexity TODO
  /// \return TODO
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  auto neighbor(const node_idx nIdx, const neighbor_pos pos, Across = Across{},
                   NeighborSet = NeighborSet{}) const noexcept -> node_idx {
    return find_neighbor(nIdx, pos, Across{}, NeighborSet{});
  }

  /// At which position of \p nIdx is \p nghbrIdx located
  ///
  /// \param [in] nIdx     node whose neighbors will be searched
  /// \param [in] nghbrIdx node to search for within \p nIdx neighbors
  ///
  /// \returns Position of \p nghbrIdx within node \p nIdx neighbors,
  /// an invalid position is returned if \p nghbrIdx is not found.
  template <class Across = across_faces_t, class NeighborSet = at_same_level_t>
  auto at_which_position(const node_idx nIdx, const node_idx nghbrIdx,
                             Across = Across{},
                             NeighborSet = NeighborSet{}) const noexcept -> neighbor_pos {
    assert_in_use(nIdx, AT_);
    assert_in_use(nghbrIdx, AT_);
    auto pos = neighbor_pos{0};
    for (const auto& nghbr : neighbors(nIdx, Across{}, NeighborSet{}, lazy{})) {
      // most neighbors are not the neighbor sought
      if (HOM3_UNLIKELY(nghbr == nghbrIdx)) { return pos; }
      ++pos;
    }
    pos = invalid;  // not found, returns invalid position

    // Consistency check:
    ASSERT(
     [&]() {
       if (nIdx == nghbrIdx) { return true; }
       using std::to_string;
       const auto conjugate_pos
        = at_which_position(nghbrIdx, nIdx, Across{}, NeighborSet{});
       if (is_valid(pos)) {
         // pos == valid, conjugate which_neighbor should return opposite
         // pos
         ASSERT(
          conjugate_pos
          == opposite_neighbor_position(pos, Across{}, NeighborSet{}),
          "nIdx = " << nIdx << " doesn't have nghbrIdx = " << nghbrIdx
                    << " in the opposite neighbor position "
                    << opposite_neighbor_position(pos, Across{}, NeighborSet{})
                    << " but in the position " << conjugate_pos << "!");
       } else {
         // is_valid(pos), conjugate which_neighbor should return invalid
         ASSERT(
          conjugate_pos == pos,
          "nIdx = " << nIdx << " doesn't have nghbrIdx = " << nghbrIdx
                    << " as neighbor, but nghbrIdx has nIdx as neighbor in pos "
                    << conjugate_pos << "!");
       }
       return true;
     }(),
     "consistency check");
    return pos;
  }

  /// Is the tree sorted?
  ///
  /// \complexity O(N)
  ///
  auto is_sorted() const noexcept -> bool {
    if (!is_compact()) { return false; }
    auto sgs = sibling_groups();
    auto dfs = dfs_siblings();
    while (dfs.next()) {
      sgs.drop_front();
      auto dfs_sg = dfs();
      auto mem_sg = *std::begin(sgs);
      if (dfs_sg != mem_sg) { return false; }
    }
    return true;
  }

  auto find_sibling_group_end(sibling_group_idx end) const noexcept -> sibling_group_idx {
    auto end_m_one = end - sibling_group_idx{1};
    while(end > sibling_group_idx{0}) {
      if (is_in_use(end_m_one)) {
        break;
      }
      --end;
      --end_m_one;
    }
    if (is_free(end_m_one)) { return sibling_group_idx{0}; }
    return end;
  }

  /// Is the \f$\#\f$ of nodes equalto the \f$\#\f$ of active nodes?
  auto is_compact() const noexcept -> bool {
    ASSERT((no_active_sibling_groups() == sibling_group_end()
            && first_free_sibling_group() == sibling_group_end())
           || (no_active_sibling_groups() != sibling_group_end()
               && first_free_sibling_group() != sibling_group_end()),
           "invariant not fulfilled: if size == #nodes then the last free "
           "sibling group must be at one past the end. Otherwise, the last "
           "free sibling group has to be before one past the end");

    return no_active_sibling_groups() == sibling_group_end();
  };

  /// Are two hyperoctrees equal?
  friend auto operator==(this_type const& l, this_type const& r) -> bool {
    return l.no_nodes() == r.no_nodes()
           && l.node_end() == r.node_end()
           && l.node_capacity() == r.node_capacity()
           && l.first_free_sibling_group() == r.first_free_sibling_group()
           && ranges::all_of(ranges::zip(l.nodes(), r.nodes()),
                             [&](auto&& ns) {
                const auto ln = boost::get<0>(ns);
                const auto rn = boost::get<1>(ns);
                return ln == rn && l.is_in_use(ln) == r.is_in_use(rn)
                       && l.parent(l.sibling_group(ln))
                          == r.parent(l.sibling_group(rn))
                       && ranges::all_of(l.child_positions(), [&](auto&& p) {
                            return l.child(ln, p)
                                   == r.child(rn, p);
                          });
              });
  }



  public:
  /// Is the tree serializable? That is, the tree:
  /// - can't be empty
  /// - must be sorted, and
  /// - must be compact.
  bool is_serializable() const {
    return !empty() && is_compact() && is_sorted();
  }

 private:
  /// Is node \p nIdx balanced across the neighbor position \p p
  ///
  /// \todo clean (and test) this up, was done in a hurry
  bool is_node_balanced(const node_idx nIdx, const neighbor_pos p,
                        bool boundary = false) const {
    if (is_root(nIdx)) { return true; }
    auto has_neighbor_in_position
     = is_valid(neighbor(nIdx, p, across_all, at_same_level));
    if (has_neighbor_in_position) { return true; } else {
      auto pIdx = parent(nIdx);
      auto parent_has_neighbor_in_position
       = is_valid(neighbor(pIdx, p, across_all, at_same_level));
      if (!boundary) {
        return parent_has_neighbor_in_position
               || (!parent_has_neighbor_in_position
                   && is_node_balanced(pIdx, p, true));
      } else {
        return !parent_has_neighbor_in_position
               && is_node_balanced(pIdx, p, true);
      }
    }
  }

  bool is_node_balanced(const node_idx nIdx) const {
    return ranges::all_of(
     neighbor_positions(across_all, at_same_level),
     [&](const auto p) { return is_node_balanced(nIdx, p); });
  }

 public:
  bool is_balanced() const {
    return ranges::all_of(
        nodes() | in_use(), [&](const auto nIdx) { return is_node_balanced(nIdx); });
  }



  ///@}  // Non-modifying queries
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name Modifying algorithms
  ///@{
 public:
  /// Refines the _leaf node_ \p nIdx isotropically, and returns the
  /// index of its first child (invalid<node_idx> if allocation failed).
  ///
  /// \param [in] nIdx node_idx to be refined.
  ///
  /// \returns node_idx of \p nIdx first child, invalid<node_idx> if children
  ///          could not be allocated.
  ///
  /// \pre \p nIdx is an _active_, _leaf node_.
  ///
  /// \post \p nIdx will not be _leaf node_ anymore since it has children.
  /// \post \p nIdx's children will be _leaf nodes_ (have no children).
  /// \post \p nIdx's children will have \p nIdx as parent.
  /// \post node_end() will be incremented by no_children() (i.e. the amount of
  ///       children created).
  /// \post size() will incremented _iff_ there are no free spots available.
  ///
  /// \invariant the tree remains balanced afterwards.
  /// \invariant all iterators remain valid afterwards.
  /// \invariant strong-guarantee: if allocation fails, no observable effects.
  ///
  /// \complexity O(1)
  ///
  auto refine_node(const node_idx nIdx) noexcept -> ranges::range<node_idx> {
    using namespace ranges;
    ASSERT(is_leaf(nIdx), "can't refine node " << nIdx << " since it is not a leaf node");
    ASSERT(level(nIdx) + 1 < max_no_levels(),
           "can't refine > max_no_levels()!");

    /// Allocate memory (and check if it failed):
    const auto child_group = allocate_siblings();
    if (!is_valid(child_group)) {
      /// \todo this should be configurable
      ASSERT(false, "failed to refine node (run out of memory?)."
                    << " Node capacity is: " << node_capacity()
             << ". Size is: " << no_nodes()
                    << ". Remaining capacity (capacity - size) is: "
             << node_capacity() - no_nodes() << ".");
      return {};
    }

    ASSERT(is_in_use(child_group), "Recelty allocated children group must be "
           "activated (otherwise allocation failed)!");
    ASSERT(is_reseted(child_group), "Future children group must be reseted!");

    /// Set children group -> parent edge:
    children_group_(nIdx) = child_group;
    /// Set parent -> children group edge:
    parent_(child_group) = nIdx;

    /// Refine neighboring nodes if necessary to ensure that the tree is always
    /// balanced:
    if (!is_root(nIdx)) {
      for (auto childIdx : all_children(nIdx)) { balance_node(childIdx); }
    }

    ASSERT([&]() {  // Assert that the children exist and are fine
             for (const auto& cIdx : all_children(nIdx)) {
               assert_in_use(cIdx, AT_);
               ASSERT(is_leaf(cIdx), "child " << cIdx << " of recently refined node "
                      << nIdx << " is not a leaf node! Has child");
             }
             return true;
           }(),
           "post-condition: new children must be active leafs");

    ASSERT(is_node_balanced(nIdx), "parent node must be balanced");
    ASSERT([&]() {
             for (const auto& cIdx : all_children(nIdx)) {
               ASSERT(is_node_balanced(cIdx),
                      "children must be balanced after refinement");
             }
             return true;
           }(),
           "all children must be balanced after refinement");
    return siblings(children_group(nIdx));
  }

 private:
  /// Balances node \p nIdx across the all_neighbor direction \p d
  ///
  /// \precondition node \p nIdx, its siblings, and their neighbors are the only
  ///               unbalanced nodes in the tree.
  ///
  /// - Go to parent of parent's neighbor in direction \p d
  ///   -> If it has a neighbor, it must be a leaf node, so refine it.
  ///   -> Otherwise, it must be a boundary node up to the root node
  ///      (if not, the 2:1 invariant has been broken somewhere else).
  ///
  auto balance_node(const node_idx nIdx, const neighbor_pos d) -> void {
    assert_in_use(nIdx, AT_);

    /// If the node is balanced there is nothing to do:
    if (is_node_balanced(nIdx, d)) { return; }
    ASSERT(!is_valid(neighbor(nIdx, d, across_all, at_same_level)),
           "otherwise the node would be balanced");
    ASSERT(!is_root(nIdx), "otherwise the node would be balanced");

    const auto pIdx = parent(nIdx);
    ASSERT(!is_root(pIdx), "otherwise the node would be balanced");
    ASSERT(!is_valid(neighbor(pIdx, d, across_all, at_same_level)),
           "otherwise the node would be balanced");

    const auto ppIdx = parent(pIdx);
    ASSERT(is_node_balanced(ppIdx, d),
           "otherwise the 2:1 invariant has been broken somewhere else already "
           "and we should fix it there and not here");
    auto neighbor_to_refine = neighbor(ppIdx, d, across_all, at_same_level);
    ASSERT(is_valid(neighbor_to_refine),
           "unexpected: we are at a boundary "
           "node but is_node_balanced missed it?");
    ASSERT(is_leaf(neighbor_to_refine),
           "if it would have children the node would have been balanced");
    refine_node(neighbor_to_refine);
  }

  /// Balances node \p nIdx across all_neighbors
  ///
  /// \precondition node \p nIdx, its siblings, and their neighbors are the only
  ///               unbalanced nodes in the tree.
  ///
  auto balance_node(const node_idx nIdx) -> void {
    assert_in_use(nIdx, AT_);
    for (neighbor_dir : neighbor_positions(across_all, at_same_level)) {
      balance_node(nIdx, neighbor_dir);
    }
  }

 public:
  /// Coarsens the node \p pIdx by removing all its children.
  ///
  /// \param [in] pIdx node_idx to be coarsened.
  ///
  /// \pre \p pIdx is an _active_ node.
  /// \pre \p pIdx's children are _leaf nodes_ (active or inactive).
  ///
  /// \post \p nIdx will be a _leaf node_ (it won't have children).
  /// \post node_end() will be decremented by the number of \p pIdx active
  ///       children.
  /// \post size() will be decremented _iff_ \p pIdx's child at position
  ///       no_children() equals size(), otherwise free_positions_ will
  ///       contain the position of \p pIdx's first child (i.e. child(pIdx, 0)).
  ///
  /// \invariant all iterators not pointing to \p pIdx's children remain valid.
  /// \invariant the tree remains balanced afterwards.
  /// \invariant strong-guarantee: if coarsening would unbalance the tree,
  ///            no observable effects.
  ///
  /// \complexity O(1)
  ///
  /// \todo Return true if coarsening succeeded, false otherwise
  auto coarse_node(const node_idx pIdx) noexcept -> bool {
    assert_in_use(pIdx, AT_);
    ASSERT(ranges::none_of(all_children(pIdx),
                   [&](const node_idx i) { return has_children(i); }),
           "can't coarsen: " << pIdx << " since its children aren't leafs");

    /// Store unique valid neighbors of pIds children (excluding siblings)
    stack::vector<node_idx, no_children() * no_neighbors(across_all, at_same_level)>
        child_neighbors;
    for (auto child : all_children(pIdx)) {
      for (auto neighbor : neighbors(child, across_all, at_same_level)) {
        child_neighbors.push_back(neighbor);
      }
    }
    // Remove children, invalid nodes, and dupplicates from the vector:
    boost::range::unique(ranges::sort(ranges::remove_erase_if(child_neighbors,
    [&](auto n) {
      return !is_valid(n)
          || ranges::any_of(all_children(pIdx), [n](auto c) { return c == n;
          });
    })));

    /// Deallocate the sibling group: this resets
    /// children group -> parent, and children group -> childrens edges
    deallocate_siblings(children_group(pIdx));

    /// Remove edge parent -> children group
    children_group_(pIdx) = invalid;

    /// Tree must remain balanced after coarsening:
    ASSERT(is_node_balanced(pIdx), "parent node must be balanced");
    if (ranges::all_of(child_neighbors, [&](auto n) { return
    is_node_balanced(n); })) {
      return true;
    } else {
      ASSERT(false,
             "this should be a roll back operation, a better way would "
             "be if the coarsening can be done a priory and if it can then "
             "coarsen otherwise don't coarsen. This also doesn't handle the "
             "data yet.");
      refine_node(pIdx);
      return false;
    }
  }

  /// Refines the tree starting at \p n until the predicate \p p is true
  /// for all its the leaf nodes
  template <class Predicate>
  void refine_node_until(const node_idx n, Predicate&& p) {
    ASSERT(is_leaf(n), "Only leaf nodes can be refined");
    ASSERT(!p(n), "wrong predicate");
    for (auto c : sub_tree(n)) {
      if (!p(c)) {
        if (is_leaf(c) && !is_valid(refine_node(c))) {
          TERMINATE("Couldn't refine node with idx " + to_string(c)
                    + ". The node capacity of the tree is "
                    + std::to_string(node_capacity()) + ".");
        }
      }
    }
  }

  //  /// Shrinks the container to the minimum possible size
  // ///
  // /// \complexity O(N)
  // /// \todo this code is probably extremely inefficient, there
  // /// must be a clever way!
  // ///
  // /// \post is_compact() returns true
  // /// \post size() == node_end()
  // auto shrink() noexcept -> void {
  //   using namespace ranges;
  //   auto firstFreeNode_ = first_sibling(firstFreeSiblingGroup_)();
  //   /// Do nothing if the container is already compact
  //   if (is_compact()) { return; }
  //   auto remainder = ranges::make<Ind>(firstFreeNode_, size())
  //                    | strided(no_children());
  //   ASSERT((size() - firstFreeNode_) % no_children() == 0, "?!");

  //   while (!remainder.empty()) {
  //     /// find first group of nodes after firstFreeNode_ that are not free
  //     const auto first_children_in_use = find_if(remainder, [&](auto i) {
  //       return any_of(siblings(node_idx{i}),
  //                     [&](auto j) { return is_in_use(j); });
  //     });

  //     /// not found? we are done!
  //     if (first_children_in_use == std::end(remainder)) { break; } else {
  //       /// swap those nodes with firstFreeNode_
  //       swap_siblings(sibling_group(node_idx{*first_children_in_use}),
  //                     sibling_group(node_idx{firstFreeNode_}));
  //       /// find next firstFreeNode
  //       firstFreeNode_ = *find_if(remainder, [&](auto i) {
  //         return all_of(siblings(node_idx{i}),
  //                       [&](auto j) { return is_inactive(j); });
  //       });
  //       firstFreeSiblingGroup_ = sibling_group(node_idx{firstFreeNode_});
  //       /// advance remainder:
  //       remainder = ranges::make<Ind>(firstFreeNode_, size())
  //                   | strided(no_children());
  //     }
  //   }
  //   // check that all nodes between firstFreeNode_ and size() are inactive
  //   ASSERT(all_of(ranges::make<node_idx>(firstFreeNode_, size()),
  //                 [&](auto i) { return is_inactive(i); }),
  //          "unexpected");
  //   // shrink size:
  //   size_() = firstFreeNode_;
  //   ASSERT(is_compact(), "something went wrong!");
  // }

  /// Resorts the tree in DFS order (results in a compact tree).
  ///
  /// \todo this works but it is the worst sort algorithm ever
  auto sort() -> void {
    bool done = false;
    while(!done) {
      done = true;
      auto sgs = sibling_groups();
      auto dfs = dfs_siblings();
      while (dfs.next()) {
        sgs.drop_front();
        auto dfs_sg = dfs();
        auto mem_sg = *std::begin(sgs);
        if (dfs_sg != mem_sg) {
          swap_siblings(dfs_sg, mem_sg);
          done = false;
          break;
        }
      }
    }
    /// Shrink end:
    sibling_group_end_() = find_sibling_group_end(sibling_group_end());
    ASSERT(first_free_sibling_group() == sibling_group_end(), "??");
    ASSERT(is_sorted(), "tree must be sorted after sort!");
    ASSERT(is_compact(), "tree must be shrunk after sort!");
    ASSERT(is_balanced(), "tree must always be balanced");
 }


  ///@}  // Modifying algorithms
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// \name I/O
  ///@{
 public:
  /// Writes this hyperoctree to the file \p file_name
  auto write(const String file_name,
             boost::mpi::communicator comm = boost::mpi::communicator()) const -> void {
    ASSERT(is_serializable(), "tree is not serializable!");

    auto output_tree = io::data::file_w{file_name, comm};

    // Write attributes: dimension and size:
    output_tree.attribute("tree_dimension", nd_);
    output_tree.attribute("tree_no_nodes", no_nodes()());

    // Write parent and children indices
    output_tree.array("/sibling_to_parent_edges",
                      reinterpret_cast<const Ind*>(sibling_group_to_parent_edges_.data()),
                      no_active_sibling_groups()());
    output_tree.array("/parent_to_children_edges",
                      reinterpret_cast<const Ind*>(parent_to_children_group_edges_.data()),
                      no_nodes()());

    ASSERT([&]() {
        for (auto i : sibling_groups()) {
          assert_in_use(i, "compact tree => all sibling groups are active!");
        }
        return true;
      }(), "all sibling groups must be active since the tree is compact!");
  }

  /// Reads hyperoctree from the file \p file_name
  auto read(const String file_name,
            boost::mpi::communicator comm = boost::mpi::communicator()) -> void {
    using std::to_string;
    auto serialized_tree = io::data::file_r{file_name, comm};

    // read attributes: dimensions, size
    const auto tree_dimension
     = serialized_tree.attribute<SInd>("tree_dimension");
    const auto tree_no_nodes = serialized_tree.attribute<Ind>("tree_no_nodes");

    // Serialized tree must have the same dimensions as output tree
    if (nd != tree_dimension) {
      TERMINATE("reading serialized tree: \"" + file_name + "\" with nd = "
                + to_string(tree_dimension)
                + " into a tree of dimension nd = " + to_string(nd));
    }

    // Tree's capacity must be large enough to hold the serialized tree
    if (node_capacity()() < tree_no_nodes) {
      TERMINATE("reading serialized tree: \"" + file_name + "\" of size = "
                + to_string(tree_no_nodes) + " into a tree with capacity = "
                + to_string(node_capacity()) + " < size: "
                + to_string(tree_no_nodes));
    }

    /// Reset tree:
    reset();

    /// Set tree bounds:
    no_active_sibling_groups_() = no_sibling_groups(tree_no_nodes);
    sibling_group_end_() = no_active_sibling_groups();
    first_free_sibling_group_() = sibling_group_end();

    /// Read tree: parent_ids, children_ids,
    serialized_tree.array("/sibling_to_parent_edges",
                          reinterpret_cast<Ind*>(sibling_group_to_parent_edges_.data()));
    serialized_tree.array("/parent_to_children_edges",
                          reinterpret_cast<Ind*>(parent_to_children_group_edges_.data()));

    /// Activate nodes:
    for (auto&& i : sibling_groups()) {
      is_in_use_(i) = true;
    }

    // resulting tree must be serializable:
    ASSERT(is_serializable(), "tree is not serializable!");
  }

  ///@}  // I/O
  //////////////////////////////////////////////////////////////////////////////

  /// \name Debugging utilities
  ///@{
 private:
  auto print_node(const node_idx nIdx) const noexcept -> void {
    std::cerr << "nIdx: " << nIdx << " | parent: " << parent(nIdx)
              << " | children: ";
    for (auto p : child_positions()) { std::cerr << child(nIdx, p) << " "; }
    std::cerr << " | childrenActive: ";
    for (auto p : child_positions()) {
      std::cerr << (is_valid(child(nIdx, p)) ? is_in_use(child(nIdx, p))
                                             : false) << " ";
    }
    std::cerr << std::endl;
  }

  auto print_siblings(const sibling_group_idx sgIdx) const noexcept -> void {
    std::cerr << "sibling group: " << sgIdx << " | siblings: ";
    for (auto c : siblings(sgIdx)) { std::cerr << c << " "; }
    std::cerr << " | parent: " << parent(sgIdx);
    std::cerr << " | first child: ";
    for (auto c : siblings(sgIdx)) { std::cerr << children_group(c) << " "; }
    std::cerr << " | in_use: " << is_in_use(sgIdx);
    if (is_valid(parent(sgIdx))) {
      std::cerr << " | parent childs: ";
      for (auto c : child_positions()) {
        const auto pIdx = parent(sgIdx);
        if (is_valid(pIdx)) {
          std::cerr << child(pIdx, c) << " ";
        } else {
          std::cerr << node_idx{invalid} << " ";
        }
      }
    } else {
      std::cerr << " | invalid_parent";
    }

    std::cerr << std::endl;
  }
 public:
  template<class Ostream>
  void pretty_print(Ostream&& os) {
    using std::to_string;
    os << "strict digraph graphname {\n";
    os << "concentrate=true;\n";

    const auto sibling_group_label = [](const sibling_group_idx sg) {
      return "g" + to_string(sg);
    };

    const auto sibling_label = [](const node_idx s) {
      return "s" + to_string(s);
    };

    const auto node_label = [=](const sibling_group_idx sg, const node_idx nIdx) {
      return sibling_group_label(sg) + ":" + sibling_label(nIdx);
    };

    // write sibling groups:
    for (auto n : sibling_groups()) {
      os << sibling_group_label(n);
      os << "[label=\"";
      os << "<gg" << n() << "> " << to_string(n) << "* |";
      auto count = 1;
      const auto last = no_children();
      for (auto s : siblings(n)) {
        os << "<" << sibling_label(s) << "> " << to_string(s);
        if (s != node_idx{0} && count != last) { os << "|"; }
        count++;
      }
      os << "\", shape=record];\n";
    }

    // write links from parent<->children
    for (auto n : sibling_groups()) {
      for (auto s : siblings(n)) {
        auto cIdx = child_(s);
        if (is_valid(cIdx)) {
          auto cg = sibling_group(cIdx);
          os << node_label(n, s)
             << " -> "
             << sibling_group_label(cg)
             << " [dir=\"both\"];\n";
        }
      }
    }

    os << "}\n";
  }

 template<class Ostream>
  void pretty_print_full(Ostream&& os) {
    using std::to_string;
    os << "digraph graphname {\n";
    os << "concentrate=true\n";

    const auto sibling_group_label = [](const sibling_group_idx sg) {
      return "g" + to_string(sg);
    };

    const auto sibling_label = [](const node_idx s) {
      return "s" + to_string(s);
    };

    const auto node_label = [=](const sibling_group_idx sg, const node_idx nIdx) {
      return sibling_group_label(sg) + ":" + sibling_label(nIdx);
    };

    // write sibling groups:
    for (auto n : sibling_groups() | ranges::filtered([&](auto i) { return is_in_use(i); })) {
      os << sibling_group_label(n);
      os << "[label=\"";
      os << "<gg" << n() << "> " << to_string(n) << "* |";
      auto count = 1;
      const auto last = no_children();
      for (auto s : siblings(n)) {
        os << "<" << sibling_label(s) << "> " << to_string(s);
        if (s != node_idx{0} && count != last) { os << "|"; }
        count++;
      }
      os << "\", shape=record];\n";
    }

    // write links from parent->children
    for (auto n : sibling_groups() | ranges::filtered([&](auto i) { return is_in_use(i); })) {
      for (auto s : siblings(n)) {
        for (auto c : child_positions()) {
          const auto cIdx = child(s, c);
          if (is_valid(cIdx)) {
            os << node_label(n, s)
               << " -> "
               << node_label(sibling_group(cIdx), cIdx)
               << ";\n";
          }
        }
      }
    }

    /// write links from children->parent
    for (auto n : nodes() | in_use()) {
      if (n == node_idx{0}) { continue; }
      const auto nsgIdx = sibling_group(n);
      const auto pIdx = parent(n);
      const auto psgIdx = sibling_group(pIdx);
      os << node_label(nsgIdx, n) << " -> " << node_label(psgIdx, pIdx) << ";\n";
    }

    os << "}\n";
  }
  ///@}  // Debugging utilities
};

}  // namespace hyperoctree
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
// File macros:
#undef ENABLE_DBG_
////////////////////////////////////////////////////////////////////////////////
