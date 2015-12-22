#pragma once
///
/// TODO:
/// - replace static_cast<int_t> with static_cast<uint_t>
///
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>
#include <memory>

namespace hm3 {
namespace tree {

/// Nd-octree data-structure
template <uint_t Nd> struct tree : geometry::dimensional<Nd> {
  /// \name Data (all member variables of the tree)
  ///
  /// Memory layout: siblings (node with the same parent) are stored
  /// contiguously in memory in Morton Z-Curve order.
  ///
  /// The order of groups of children is arbitrary.
  ///
  /// Memory requirements: 1 word + 1 / no_children word per node
  /// - each node stores the index of its first child (the other children are
  ///   stored contiguously after the first in Z-Order)
  /// - each group of siblings stores the index of its parent
  ///
  /// \warning the interanals are public by design (e.g. for extensible
  /// serialization) but unstable (i.e. subjected to change without prior
  /// notice).
  ///
  ///@{

  /// Sibling group capacity: maximum number of sibling groups that the tree can
  /// store
  siblings_idx sg_capacity_ = 0_sg;
  /// Indices to the parent node of each sibling group (1 index / sibling group)
  std::unique_ptr<node_idx[]> parents_ = nullptr;
  /// Indices of the first children of each node (1 index / node)
  std::unique_ptr<node_idx[]> first_children_ = nullptr;
  /// Number of nodes in the tree
  node_idx size_ = 0_n;
  /// First group of siblings that is free (i.e. not in use)
  siblings_idx first_free_sibling_group_{0};

  ///@}  // Data

 public:
  /// \name Spatial constants
  ///@{

  using geometry::dimensional<Nd>::dimension;
  using geometry::dimensional<Nd>::dimensions;

  /// Number of children per node
  static constexpr uint_t no_children() noexcept {
    return hm3::tree::no_children(Nd);
  }

  /// Position of node \p n within its parent
  ///
  /// \post n == child(parent(n), position_in_parent(n))
  ///
  static constexpr uint_t position_in_parent(node_idx n) noexcept {
    return ((*n) - 1_u) % no_children();
    // post-condition is recursive and cannot be asserted
  }

  ///@}  // Spatial constants

  /// \name Graph edges (parent/children)
  ///
  ///@{

  /// Index of the sibling group of node \p n
  static constexpr siblings_idx sibling_group(node_idx n) noexcept {
    HM3_ASSERT(n, "cannot compute sibling group of invalid node");
    return siblings_idx{static_cast<int_t>(
     math::floor((*n + no_children() - 1.f) / no_children()))};
  }

  /// Index of the parent node of the sibling group \p s
  node_idx parent(siblings_idx s) const noexcept {
    HM3_ASSERT(s, "cannot obtain parent of invalid sibling group");
    HM3_ASSERT(s >= 0_sg and s < sibling_group_capacity(),
               "sg {} is out-of-bounds for parents [{}, {})", s, 0,
               sibling_group_capacity());
    return parents_[*s];
  }

 private:
  /// Sets the index of the parent node of the sibling group \p s to \p value
  ///
  /// \post parent(s) == value
  ///
  /// \warning not thread-safe (requires external synchronization)
  void set_parent(siblings_idx s, node_idx value) noexcept {
    HM3_ASSERT(s, "cannot set parent of invalid sibling group");
    HM3_ASSERT(s >= 0_sg and s < sibling_group_capacity(),
               "sg {} is out-of-bounds for parents [{}, {})", s, 0,
               sibling_group_capacity());
    parents_[*s] = value;
    HM3_ASSERT(parent(s) == value, "");
  }

 public:
  /// Index of the parent node of node \p n
  node_idx parent(node_idx n) const noexcept {
    return parent(sibling_group(n));
  }

  /// Child position type is a uint_t bounded in [0, no_children)
  using child_pos = ::hm3::tree::child_pos<Nd>;

 private:
  /// Index of the first children of node \p n
  ///
  /// \post !c || first_child(n) == child(n, 0)
  /// \post !c || parent(first_child(n)) == n
  ///
  node_idx first_child(node_idx n) const noexcept {
    HM3_ASSERT(n, "cannot obtain first child of invalid node");
    HM3_ASSERT(n >= 0_n and n < capacity(),
               "node {} is out-of-bounds for first_child [{}, {})", n, 0,
               capacity());
    return first_children_[*n];
    // cannot assert post-condition because swap temporarily violates it
  }

  /// First node in sibling group \p s
  static constexpr node_idx first_node(siblings_idx s) noexcept {
    return (*s == 0) ? 0_n : node_idx{1 + no_children() * (*s - 1)};
  }

  /// Sets the index of the first child of node \p n to \p value
  ///
  /// \post child(n, 0) == value
  ///
  /// \warning not thread-safe
  void set_first_child(node_idx n, node_idx value) noexcept {
    HM3_ASSERT(n, "cannot set first child of invalid node");
    HM3_ASSERT(n >= 0_n and n < capacity(),
               "node {} is out-of-bounds for first_child [{}, {})", n, 0,
               capacity());
    first_children_[*n] = value;
    HM3_ASSERT(child(n, child_pos{0}) == value, "");
  }

 public:
  /// Index of the group of children of node \p n
  siblings_idx children_group(node_idx n) const noexcept {
    auto c = first_child(n);
    return c ? sibling_group(c) : siblings_idx{};
  }

  /// Range of child positions: [0, no_children)
  static constexpr auto child_positions() noexcept { return child_pos::rng(); }

  /// Child node at position \p p of node \p n
  node_idx child(node_idx n, child_pos p) const noexcept {
    const auto fc = first_child(n);
    return fc ? node_idx{*fc + *p} : fc;
  }

  /// Range of children nodes of node \p n
  auto children(node_idx n) const noexcept {
    const auto fc = first_child(n);
    return fc ? boxed_ints<node_idx>(*fc, *fc + no_children())
              : boxed_ints<node_idx>(0_n, 0_n);
  }

 private:
  /// First node of the tree
  static constexpr node_idx first_node() noexcept { return 0_n; }
  /// Last node of the tree
  node_idx last_node() const noexcept { return capacity(); }

  /// First sibling group of the tree
  static constexpr siblings_idx first_sg() noexcept { return 0_sg; }
  /// Last sibling group of the tree
  constexpr siblings_idx last_sg() const noexcept {
    return sibling_group(last_node());
  }

 public:
  /// Nodes in sibling group \p s
  static constexpr auto nodes(siblings_idx s) noexcept {
    const auto fn = first_node(s);
    return !is_root(s) ? boxed_ints<node_idx>(*fn, *fn + no_children())
                       : boxed_ints<node_idx>(0_n, 1_n);
  }

  /// Nodes in the sibling group of \p n (i.e. includes n itself)
  static constexpr auto siblings(siblings_idx s) noexcept { return nodes(s); }

  /// Nodes in the sibling group of \p n (i.e. includes n itself)
  static constexpr auto siblings(node_idx n) noexcept {
    return nodes(sibling_group(n));
  }

 public:
  /// Sets the first free sibling group to \p s
  void set_first_free_sibling_group(siblings_idx s) noexcept {
    HM3_ASSERT(s, "cannot set the first free sibling group to empty");

    HM3_ASSERT(s == sibling_group_capacity() || is_free(s),
               "sibling group {} is not free", s);
    first_free_sibling_group_ = s;
    HM3_ASSERT(
     all_of(boxed_ints(0_sg, s), [&](siblings_idx i) { return !is_free(i); }),
     "invalid first free sibling: found free siblings in range [0, {})", s);
  }

 public:
  /// All nodes in use within the tree (fast and safe)
  ///
  /// \pre the tree must be compact
  auto operator()() const noexcept {
    HM3_ASSERT(is_compact(), "fast node range requires compact tree");
    return boxed_ints<node_idx>(first_node(), last_node());
  }

  /// All nodes in use within the tree (slow and safe)
  ///
  /// \note The tree does not need to be compact
  auto nodes() const noexcept {
    return sibling_groups()  // range of all non-free sibling groups
           // map each sibling group into a range of its nodes
           // (which produces a range of ranges of nodes):
           | view::transform([](siblings_idx s) { return nodes(s); })
           // flatten the range into a range of nodes:
           | view::join;
  }

  /// Range filter that selects leaf nodes only
  auto leaf() const noexcept {
    return view::filter([&](node_idx i) { return is_leaf(i); });
  }

  /// Range filter that selects nodes with children only
  auto with_children() const noexcept {
    return view::remove_if([&](node_idx i) { return is_leaf(i); });
  }

 private:
  /// All non-free sibling group indices in the tree
  auto sibling_groups() const noexcept {
    // range of all sibling groups from [0, sibling_group_capacity)
    return boxed_ints<siblings_idx>(first_sg(), last_sg())
           // select those sibling groups that are not free
           | view::filter([&](siblings_idx s) { return !is_free(s); });
  }

 private:
  /// Is \p s the sibling group of the root node?
  static constexpr bool is_root(siblings_idx s) noexcept { return *s == 0; }

 public:
  /// Is \p n the root node?
  static constexpr bool is_root(node_idx n) noexcept { return *n == 0; }

 private:
  /// A sibling group is free if it doesn't have a parent
  /// (unless it is the root node, which is never empty)
  bool is_free(siblings_idx s) const noexcept {
    if (HM3_UNLIKELY(is_root(s))) { return false; }
    return !parent(s);
  }

 protected:
  /// Is node \p n part of a free sibling group?
  bool is_free(node_idx n) const noexcept { return is_free(sibling_group(n)); }

 public:
  /// Is node \p n a leaf node? (That is, does it have zero children?)
  bool is_leaf(node_idx n) const noexcept { return !first_child(n); }

  /// Number of childrens of the node \p n
  uint_t no_children(node_idx n) const noexcept {
    return is_leaf(n) ? 0 : no_children();
  }

  /// Does the tree have a compact representation?
  ///
  /// That is, no free sibling groups before the last sibling group in use.
  ///
  bool is_compact() const noexcept {
    const auto result = first_free_sibling_group_ == sibling_group(size());

    auto no_free_sgs = [&]() {
      return none_of(
       boxed_ints<siblings_idx>(first_sg(), sibling_group(size())),
       [&](auto&& s) { return is_free(s); });
    };
    HM3_ASSERT((result and no_free_sgs()) or (!result and !no_free_sgs()),
               "first free sibling group is wrong");

    return result;
  }

  ///@}  // Graph edges (parent/children)

  /// \name Memory management
  ///@{

  /// Number of sibling groups required to hold \p no_nodes nodes
  static constexpr siblings_idx no_sibling_groups(node_idx no_nodes) noexcept {
    return siblings_idx{
     no_nodes > 0 ? *sibling_group(node_idx{*no_nodes - 1}) + 1 : 0};
  }

  /// Number of nodes that can be holded by \p no_sibling_groups sibling groups
  static constexpr node_idx no_nodes(siblings_idx no_sibling_groups) noexcept {
    switch (*no_sibling_groups) {
      case 0: {
        return 0_n;
      }
      case 1: {
        return 1_n;
      }
      default: {
        return node_idx{(*no_sibling_groups - 1) * no_children() + 1};
      }
    }
  }

  /// Maximum number of sibling groups that the tree can hold
  siblings_idx sibling_group_capacity() const noexcept { return sg_capacity_; }

  /// Maximum number of nodes that the tree can hold
  node_idx capacity() const noexcept {
    return no_nodes(sibling_group_capacity());
  }

 private:
  /// All parents as stored in memory
  constexpr auto all_parents() const noexcept {
    return view::counted(parents_.get(), *sibling_group_capacity());
  }

  /// All children as stored in memory
  constexpr auto all_children() const noexcept {
    return view::counted(first_children_.get(), *capacity());
  }

 public:
  /// Number of nodes in the tree
  node_idx size() const noexcept { return size_; }

  /// Is the tree empty?
  bool empty() const noexcept { return size_ == 0_n; }

  /// Refine node \p p and returns children group idx
  ///
  /// \returns sibling group of children (empty if refinement failed).
  ///
  /// \pre !is_free(p) && is_leaf(p)
  /// \post !is_free(p) && !is_leaf(p)
  siblings_idx refine(node_idx p) noexcept {
    HM3_ASSERT(!is_free(p), "node {}: is free and cannot be refined", *p);
    HM3_ASSERT(is_leaf(p), "node {}: is not a leaf and cannot be refined", *p);
    if (size() == capacity()) { return siblings_idx{}; }

    const auto s = first_free_sibling_group_;
    HM3_ASSERT(is_free(s), "node {}: first free sg {} is not free", *p, *s);

    size_ += node_idx{no_children()};

    ++first_free_sibling_group_;
    auto remaining_sgs
     = boxed_ints<siblings_idx>(first_free_sibling_group_, last_sg());
    HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(remaining_sgs);

    auto next_unused_sg_it
     = find_if(remaining_sgs, [&](siblings_idx i) { return is_free(i); });
    if (next_unused_sg_it != end(remaining_sgs)) {
      first_free_sibling_group_ = *next_unused_sg_it;
    }

    set_parent(s, p);
    set_first_child(p, first_node(s));

    HM3_ASSERT(!is_free(s), "node {}: refine produced a free sg {}", *p, *s);
    HM3_ASSERT(all_of(children(p), [&](node_idx i) { return is_leaf(i); }),
               "node {}: refine produced non leaf children", *p);
    HM3_ASSERT(!is_free(p), "node {}: refine freed the node", *p);
    HM3_ASSERT(!is_leaf(p), "node {}: refine produced a leaf parent node", *p);
    return s;
  }

  /// Coarsen node \p p
  ///
  /// \pre !is_free(p) && !is_leaf(p) && is_leaf(children group of p)
  /// \post !is_free(p) && is_leaf(p) && is_free(children group of p)
  void coarsen(node_idx p) noexcept {
    HM3_ASSERT(!is_free(p), "node {}: is free, cannot coarsen", *p);
    HM3_ASSERT(!is_leaf(p), "node {}: is leaf, cannot coarsen", *p);

    const auto cg = children_group(p);
    HM3_ASSERT(!is_free(cg), "node {}: its child group {} is free", *p, *cg);

    size_ -= node_idx{no_children()};

    if (*cg < *first_free_sibling_group_) { first_free_sibling_group_ = cg; }

    set_parent(cg, node_idx{});
    set_first_child(p, node_idx{});

    HM3_ASSERT(is_free(cg), "node {}: after coarsen child group {} not free",
               *p, *cg);
    HM3_ASSERT(is_leaf(p), "node {}: after coarsen not leaf", *p);
    HM3_ASSERT(!is_free(p), "node {}: after coarsen is free", *p);
  }

 private:
  /// Initializes the tree with a root node
  ///
  /// \pre empty()
  /// \post size() == 1
  void initialize_root_node() {
    HM3_ASSERT(empty(), "cannot initialize root node on non-empty tree");
    HM3_ASSERT(!parent(0_sg), "first sibling group has a parent");
    HM3_ASSERT(is_leaf(0_n), "root node already has children");
    ++size_;
    ++first_free_sibling_group_;
    HM3_ASSERT(size() == 1, "after root node init size is {} and not 1",
               size());
  }

  /// Is the tree reseted?
  bool is_reseted() {
    return size_ == 0 and first_free_sibling_group_ == 0_sg
           and all_of(all_parents(), [](node_idx i) { return !i; })
           and all_of(all_children(), [](node_idx i) { return !i; });
  }

 public:
  /// Swaps the memory location of the sibling groups \p a and \p b
  ///
  /// \pre no sibling group can be swapped with the root's node sibling group
  /// \pre no sibling group can be swapped with itself
  ///
  /// \warning not thread safe
  void swap(siblings_idx a, siblings_idx b) noexcept {
    HM3_ASSERT(a != 0_sg and b != 0_sg, "root node is not swappable");
    HM3_ASSERT(a || b, "at least one of both sg must be valid");
    HM3_ASSERT(a != b, "self-swap not allowed for sg {}", a ? *a : -1);

    // 0) Break early: both not in use -> nothing to do
    if (is_free(a) and is_free(b)) { return; }

    /// 1) swap siblings -> children edges, and children -> sibling edges:
    auto update_cg_parent = [&](node_idx s) {
      const auto child_cg = children_group(s);
      if (child_cg) { set_parent(child_cg, s); }
    };
    for (auto n : view::zip(nodes(a), nodes(b))) {
      auto l = get<0>(n), r = get<1>(n);
      ranges::swap(first_children_[*l], first_children_[*r]);
      update_cg_parent(l);
      update_cg_parent(r);
    };

    /// 2) swap parent -> sibling edges, and sibling -> parent edges:
    auto update_parent_sibling_e = [&](node_idx parent, siblings_idx s) {
      if (parent) { set_first_child(parent, first_node(s)); }
      set_parent(s, parent);
    };
    {
      auto p_a = parent(a);
      auto p_b = parent(b);
      update_parent_sibling_e(p_a, b);
      update_parent_sibling_e(p_b, a);
    }

    /// 3) update the first free sibling group flag:
    ///    - if one of the nodes is inactive:
    auto update_first_free_sg = [&](siblings_idx i, siblings_idx j) {
      if (is_free(i)) {
        first_free_sibling_group_
         = (j == first_free_sibling_group_) ? i : first_free_sibling_group_;
      }
    };
    update_first_free_sg(a, b);
    update_first_free_sg(b, a);
  }

  ///@}  // Memory management

 public:
  tree() = default;

  /// Creates a tree with capacity for at least \p cap nodes and initializes it
  /// with a root node
  tree(node_idx node_capacity)
   : sg_capacity_(no_sibling_groups(node_capacity))
   , parents_(std::make_unique<node_idx[]>(*sibling_group_capacity()))
   , first_children_(std::make_unique<node_idx[]>(*capacity())) {
    HM3_ASSERT(capacity() > 0_n,
               "cannot construct tree with zero capacity ({})", capacity());
    HM3_ASSERT(is_reseted(), "tree is not reseted");
    initialize_root_node();
  }

  tree(tree&& other) = default;

  tree(tree const& other) : tree(*other.capacity()) {
    size_                     = other.size_;
    first_free_sibling_group_ = other.first_free_sibling_group_;
    {  // copy parents_
      auto b = other.parents_.get();
      auto e = b + *other.sibling_group_capacity();
      auto o = parents_.get();
      copy(b, e, o);
    }
    {  // copy first_children_
      auto b = other.first_children_.get();
      auto e = b + *other.capacity();
      auto o = first_children_.get();
      copy(b, e, o);
    }
  }

  tree& operator=(tree other) {
    ranges::swap(*this, other);
    return *this;
  }
};

/// Graph equality
///
/// Two trees are equal if their parent-child graph is the same.
///
template <uint_t Nd>
bool operator==(tree<Nd> const& a, tree<Nd> const& b) noexcept {
  if (size(a) != size(b)) { return false; }

  RANGES_FOR (auto&& np, view::zip(a.nodes(), b.nodes())) {
    auto&& an = get<0>(np);
    auto&& bn = get<1>(np);
    if (a.parent(an) != b.parent(bn)) { return false; }
    if (!equal(a.children(an), b.children(bn))) { return false; }
  }
  return true;
}

template <uint_t Nd>
bool operator!=(tree<Nd> const& a, tree<Nd> const& b) noexcept {
  return !(a == b);
}

/// Binary tree
using binary_tree = tree<1>;

/// Quad-tree
using quad_tree = tree<2>;

/// Oct-tree
using oct_tree = tree<3>;

template <uint_t Nd> string type(tree<Nd> const&) { return "tree"; }

template <uint_t Nd> string name(tree<Nd> const&) {
  return type(tree<Nd>{}) + "_" + std::to_string(Nd) + "D";
}

}  // namespace tree
}  // namespace hm3
