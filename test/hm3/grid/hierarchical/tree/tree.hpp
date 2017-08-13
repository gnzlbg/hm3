#pragma once
/// \file
///
/// Tree testing utilities

#ifdef HM3_TEST_PRIVATE_AS_PUBLIC
// Don't do this at home TM:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#define private public
#include <hm3/grid/hierarchical/tree/tree.hpp>
#pragma clang diagnostic pop
#else
#include <hm3/grid/hierarchical/tree/tree.hpp>
#endif
#include <hm3/grid/hierarchical/serialization/fio.hpp>
#include <hm3/grid/hierarchical/tree/algorithm.hpp>
#include <hm3/grid/hierarchical/tree/location/deinterleaved.hpp>
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/serialization/fio.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/test.hpp>
#ifdef HM3_ENABLE_VTK
#include <hm3/grid/hierarchical/tree/serialization/vtk.hpp>
#endif

namespace hm3 {
namespace test {

using namespace hm3::tree;

/// invalid value
static const constexpr auto i = std::numeric_limits<uint_t>::max();

inline std::vector<uint_t> is(std::size_t n) {
  return std::vector<uint_t>(n, i);
}

/// Testing utility for constructing a node with named argument lists
template <typename Tag, typename T = uint_t>
struct tagged_initializer_list {
  tagged_initializer_list(std::vector<T>&& other) : data{std::move(other)} {}
  template <typename... Args, class = decltype(std::vector<T>{
                               static_cast<T>(std::declval<Args>())...})>
  tagged_initializer_list(Args&&... args) : data{static_cast<T>(args)...} {}
  std::vector<T> data;
  auto begin() noexcept { return hm3::begin(data); }
  auto begin() const noexcept { return hm3::begin(data); }
  auto end() noexcept { return hm3::end(data); }
  auto end() const noexcept { return hm3::end(data); }
  auto size() const noexcept { return hm3::size(data); }

  T operator*() const noexcept {
    if (ranges::size(data) != 1_u) {
      HM3_FATAL_ERROR(
       "cannot dereference tagged_initializer_list of size {} != 1",
       ranges::size(data));
    };
    return index(data, 0);
  }
};

/// Node Index
using idx = tagged_initializer_list<class node_idx_tag_>;
/// Node Level
using lvl = tagged_initializer_list<class level_tag_>;
/// Node's parent node
using pn = tagged_initializer_list<class parent_node_tag_>;
/// Node's children
using cs = tagged_initializer_list<class children_tag_>;
/// Position in parent from root to node
using pip = tagged_initializer_list<class position_in_parent_tag_>;
/// Face neighbors
using fn = tagged_initializer_list<class face_neighbors_tag_>;
/// Edge neighbors
using en = tagged_initializer_list<class edge_neighbors_tag_>;
/// Corner neighbors
using cn = tagged_initializer_list<class corner_neighbors_tag_>;
/// All neighbors
using an = tagged_initializer_list<class all_neighbors_tag_>;
/// Normalized coordinates
using nc = tagged_initializer_list<class normalized_coordinates_tag_, num_t>;

/// Test data for a single node
struct node {
  optional<node_idx> idx{};
  optional<uint_t> level{};
  optional<node_idx> parent{};
  optional<std::vector<node_idx>> children{};
  optional<std::vector<uint_t>> pos_in_parent{};
  optional<std::vector<node_idx>> face_neighbors{};
  optional<std::vector<node_idx>> edge_neighbors{};
  optional<std::vector<node_idx>> corner_neighbors{};
  optional<std::vector<node_idx>> all_neighbors{};
  optional<std::vector<num_t>> normalized_coordinates{};

  node()            = default;
  node(node const&) = default;
  node(node&&)      = default;
  node& operator=(node const&) = default;
  node& operator=(node&&) = default;

  template <typename Arg, typename... Args,
            CONCEPT_REQUIRES_(!ConvertibleTo<std::decay_t<Arg>, node>{})>
  void init(Arg&& arg, Args&&... args) {
    init(std::forward<Arg>(arg));
    init(std::forward<Args>(args)...);
  }

  template <typename Arg, typename... Args,
            CONCEPT_REQUIRES_(!ConvertibleTo<std::decay_t<Arg>, node>{})>
  node(Arg&& arg, Args&&... args) {
    init(std::forward<Arg>(arg), std::forward<Args>(args)...);
  }

  void init(::hm3::test::idx j) { idx = node_idx{*j}; }
  void init(lvl l) { level = *l; }
  void init(pn p) { parent = *p != i ? node_idx{*p} : node_idx{}; }
  void init(cs c) {
    children = std::vector<node_idx>(size(c));
    ranges::transform(c, begin(*children),
                      [](auto j) { return j != i ? node_idx{j} : node_idx{}; });
  }
  void init(pip ps) {
    pos_in_parent = std::vector<uint_t>(size(ps));
    ranges::transform(ps, begin(*pos_in_parent),
                      [](auto p) { return static_cast<uint_t>(p); });
  }
  void init(fn ns) {
    face_neighbors = std::vector<node_idx>(size(ns));
    ranges::transform(ns, begin(*face_neighbors),
                      [](auto c) { return c != i ? node_idx{c} : node_idx{}; });
  }
  void init(en ns) {
    edge_neighbors = std::vector<node_idx>(size(ns));
    ranges::transform(ns, begin(*edge_neighbors),
                      [](auto c) { return c != i ? node_idx{c} : node_idx{}; });
  }
  void init(cn ns) {
    corner_neighbors = std::vector<node_idx>(size(ns));
    ranges::transform(ns, begin(*corner_neighbors),
                      [](auto c) { return c != i ? node_idx{c} : node_idx{}; });
  }
  void init(an ns) {
    all_neighbors = std::vector<node_idx>(size(ns));
    ranges::transform(ns, begin(*all_neighbors),
                      [](auto c) { return c != i ? node_idx{c} : node_idx{}; });
  }
  void init(nc x) {
    normalized_coordinates = std::vector<num_t>(size(x));
    ranges::copy(x, begin(*normalized_coordinates));
  }
};

template <typename Rule>  //
node rewrite_node(node n, Rule&& r) {
  if (n.idx) { *n.idx = r(*n.idx); }
  if (n.parent) { *n.parent = r(*n.parent); }
  auto rrange = [&](auto&& opt_rng) {
    if (opt_rng) {
      for (auto&& v : *opt_rng) { v = r(v); }
    }
  };
  rrange(n.children);
  rrange(n.face_neighbors);
  rrange(n.edge_neighbors);
  rrange(n.corner_neighbors);
  rrange(n.all_neighbors);
  if (n.all_neighbors) { ranges::sort(*n.all_neighbors); }
  return n;
}

template <typename Nodes, typename Rule>
Nodes rewrite_nodes(Nodes ns, Rule&& r) {
  for (auto&& n : ns.nodes) { n = rewrite_node(n, r); }
  return ns;
}

template <typename Tree>
void test_parent(Tree const& t, node const& n) {
  // consistency:
  if (t.parent(*n.idx)) {
    CHECK(ranges::any_of(t.children(t.parent(*n.idx)),
                         [&](auto&& c) { return c == *n.idx; }));
    CHECK(!t.is_root(*n.idx));
  } else {
    CHECK(t.is_root(*n.idx));
  }
  // check parent node:
  if (n.parent) { CHECK(t.parent(*n.idx) == *n.parent); }
}

template <typename Tree>
void test_level(Tree const& t, node const& n) {
  // consistency:
  if (node_level(t, *n.idx) == 0) {
    CHECK(t.is_root(*n.idx));
  } else {
    CHECK(!t.is_root(*n.idx));
  }
  // check node level:
  if (n.level) { CHECK(node_level(t, *n.idx) == *n.level); }
}
template <typename Tree>
void test_children(Tree const& t, node const& n) {
  // consistency
  if (t.no_children(*n.idx) == 0_u) {
    CHECK(t.is_leaf(*n.idx));
  } else {
    CHECK(!t.is_leaf(*n.idx));
    for (auto p : t.child_positions()) {
      auto c = t.child(*n.idx, p);
      if (c) { CHECK(t.parent(c) == *n.idx); }
    }
  }

  if (n.children) {
    CHECK(t.no_children(*n.idx) == size(*n.children));
    test::check_equal(t.children(*n.idx), *n.children);
    for (auto p : t.child_positions()) {
      auto c = t.child(*n.idx, p);
      if (c) {
        CHECK(c == (*n.children)[*p]);
      } else {
        CHECK(size(*n.children) == 0_u);
      }
    }
  }
}

template <typename Tree, typename Location>
void test_node_location(Tree const& t, node const& n, Location l) {
  STATIC_CHECK(Tree::ambient_dimension() == Location::ambient_dimension());
  // consistency:
  CHECK(node_at(t, node_location(t, *n.idx, l)) == *n.idx);
}

template <typename Tree, typename Location>
void test_pos_in_parent(Tree const& t, node const& n, Location l) {
  STATIC_CHECK(Tree::ambient_dimension() == Location::ambient_dimension());
  if (!n.pos_in_parent) { return; }
  test::check_equal(node_location(t, *n.idx, l)(), *n.pos_in_parent);
  CHECK(node_at(t, Location(*n.pos_in_parent)) == *n.idx);
}

template <typename Tree, typename Neighbors, typename NeighborType,
          typename Location>
void test_node_neighbor(Tree const& t, node const& n, Neighbors const& ns,
                        NeighborType, Location l) {
  if (!ns) { return; }
  using neighbor_idx = neighbor_idx_t<NeighborType>;
  CHECK(size(neighbor_idx::rng()) == size(*ns));
  for (auto&& p : neighbor_idx::rng()) {
    auto neighbor = node_neighbor(t, node_location(t, *n.idx, l), p);
    CHECK(neighbor == (*ns)[*p]);
    if (neighbor) {
      CHECK(node_neighbor(t, node_location(t, neighbor, l), opposite(p))
            == *n.idx);
    }
  }
}

template <typename Tree, typename Neighbors, typename Location>
void test_node_neighbors(Tree const& t, node const& n, Neighbors const& ns,
                         Location l) {
  if (!ns) { return; }
  auto neighbors = node_neighbors(t, node_location(t, *n.idx, l));
  CHECK(size(neighbors) == size(*ns));
  test::check_equal(neighbors, *ns);
}

template <typename Tree, typename Location>
void test_normalized_coordinates(Tree const& t, node const& n, Location l) {
  auto nc = normalized_coordinates(t, *n.idx, l);
  if (n.normalized_coordinates) {
    auto reference = *n.normalized_coordinates;
    test::check_approx_equal(nc, reference);
  }
}

template <typename Tree, typename Location>
void check_node(Tree const& t, node n, Location l) {
  STATIC_CHECK(Tree::ambient_dimension() == Location::ambient_dimension());
  test_parent(t, n);
  test_level(t, n);
  test_children(t, n);
  test_node_location(t, n, l);
  test_pos_in_parent(t, n, l);
  test_node_neighbor(t, n, n.face_neighbors,
                     face_neighbors<Tree::ambient_dimension()>{}, l);
  test_node_neighbor(t, n, n.edge_neighbors,
                     edge_neighbors<Tree::ambient_dimension()>{}, l);
  test_node_neighbor(t, n, n.corner_neighbors,
                     corner_neighbors<Tree::ambient_dimension()>{}, l);
  test_node_neighbors(t, n, n.all_neighbors, l);
  test_normalized_coordinates(t, n, l);
}

template <typename Tree,
          typename Location
          = location::default_location<Tree::ambient_dimension()>>
void check_root_node_invariants(Tree const& tree) {
  const auto root_idx        = 0_n;
  const auto root_parent_idx = tree.parent(root_idx);
  CHECK(!root_parent_idx);
  auto loc = node_location(tree, root_idx, Location{});

  // all neighbors:
  {
    auto neighbors = node_neighbors(tree, loc);
    CHECK(size(neighbors) == 0_z);
  }
  // across a particular face:
  auto check_neighbors = [&](auto manifold) {
    using neighbor_idx = neighbor_idx_t<decltype(manifold)>;
    for (auto&& p : neighbor_idx::rng()) {
      auto neighbor = node_neighbor(tree, loc, p);
      CHECK(!neighbor);
    }
  };
  check_neighbors(face_neighbors<Tree::ambient_dimension()>{});
  check_neighbors(edge_neighbors<Tree::ambient_dimension()>{});
  check_neighbors(corner_neighbors<Tree::ambient_dimension()>{});
}

template <typename Tree>
void check_consistent_parent_child_edges(Tree const& tree) {
  for (auto n : tree.nodes()) {
    for (auto p : tree.child_positions()) {
      auto child = tree.child(n, p);
      if (child) { CHECK(tree.parent(child) == n); }
    }
  }
}

template <typename Tree>
void check_consistent_leaf_nodes(Tree const& tree) {
  for (auto n : tree.nodes() | tree.leaf()) {
    CHECK(ranges::distance(tree.children(n)) == 0);
    CHECK(tree.no_children(n) == 0_u);
    for (auto p : tree.child_positions()) { CHECK(!tree.child(n, p)); }
  }
  for (auto n : tree.nodes() | tree.with_children()) {
    CHECK(static_cast<uint>(ranges::distance(tree.children(n)))
          == tree.no_children());
    CHECK(tree.no_children(n) == static_cast<uint_t>(tree.no_children()));
    for (auto p : tree.child_positions()) { CHECK(tree.child(n, p)); }
  }
}

template <typename Tree,
          typename Location
          = location::default_location<Tree::ambient_dimension()>>
void check_consistent_neighbors(Tree const& tree, Location = Location{}) {
  constexpr uint_t nd = Tree::ambient_dimension();

  auto check_opposite_neighbors = [&tree](auto manifold, auto n, auto loc) {
    using neighbor_idx = neighbor_idx_t<decltype(manifold)>;
    for (auto&& p : neighbor_idx::rng()) {
      auto neighbor = node_neighbor(tree, loc, p);
      if (neighbor) {
        auto opposite_neighbor = node_neighbor(
         tree, node_location(tree, neighbor, Location{}), opposite(p));
        CHECK(opposite_neighbor == n);
      }
    }
  };

  for (auto n : tree.nodes()) {
    auto loc = node_location(tree, n, Location{});
    check_opposite_neighbors(face_neighbors<nd>{}, n, loc);
    check_opposite_neighbors(edge_neighbors<nd>{}, n, loc);
    check_opposite_neighbors(corner_neighbors<nd>{}, n, loc);
  }
}

template <typename Tree,
          typename Location
          = location::default_location<Tree::ambient_dimension()>>
void check_is_balanced(Tree const& tree, Location = Location{}) {
  for (auto n : tree.nodes()) {
    const auto loc = node_location(tree, n, Location{});
    auto neighbors = node_neighbors(tree, loc);
    for (auto neighbor : neighbors) {
      auto neighbor_loc = node_location(tree, neighbor, Location{});
      CHECK(math::absdiff(*loc.level(), *neighbor_loc.level()) < 2_u);
    }
  }
}

/// Performs all consistency checks:
template <typename Tree,
          typename Location
          = location::default_location<Tree::ambient_dimension()>>
void consistency_checks(Tree const& tree, Location = Location{}) {
  check_root_node_invariants(tree);
  check_consistent_parent_child_edges(tree);
  check_consistent_leaf_nodes(tree);
  check_consistent_neighbors(tree, Location{});
}

template <typename Tree, typename ReferenceTree,
          typename Location
          = location::default_location<Tree::ambient_dimension()>>
void check_tree(Tree const& tree, ReferenceTree const& tref,
                Location l = Location{}) {
  STATIC_CHECK(Tree::ambient_dimension() == Location::ambient_dimension());
  for (auto&& n : tref.nodes) { check_node(tree, n, l); }
  consistency_checks(tree, Location{});
}

template <typename Tree, typename ReferenceTree>
void check_tree_node_range(Tree const& t, ReferenceTree const& tref) {
  auto tref_nodes
   = tref.nodes | view::transform([](auto&& n) { return n.idx; });
  test::check_equal(t.nodes(), tref_nodes);
}

template <int Nd>
auto uniformly_refined_tree(uint_t level, uint_t level_capacity)
 -> hm3::tree::tree<Nd> {
  auto node_capacity = no_nodes_until_uniform_level(Nd, level_capacity);

  hm3::tree::tree<Nd> t(node_capacity);
  for (auto&& n : t.nodes() | t.leaf()) {
    if (node_level(t, n) < level) { t.refine(n); }
  }

  CHECK(!t.empty());
  CHECK(t.size() == no_nodes_until_uniform_level(Nd, level));

  return t;
}

template <typename Tree, typename Location = location::default_location<Tree::ambient_dimension()>>  //
Tree check_io(Tree tree, string file_name, Location = Location{}) {
  file_name = name(tree) + "_" + file_name;
  dfs_sort(tree);
  CHECK(tree.is_compact() == true);
  CHECK(dfs_sort.is(tree) == true);
  /*
  auto comm = mpi::comm::world();
  io::session::remove(file_name, comm);

  // write tree to disk:
  hierarchical::to_file(tree, file_name);
  std::cerr << "HERE0" << std::endl;
  consistency_checks(tree, Location{});
  std::cerr << "HERE1" << std::endl;

  // read tree from disk
  auto input = hierarchical::from_file(Tree{}, file_name);
  consistency_checks(input, Location{});

  CHECK(tree == input);  // both trees should be identical in memory
  CHECK(input == tree);  // both trees should be identical in memory

  // write new tree back to disk
  string input_fn = file_name + "_after_read";
  io::session::remove(input_fn, comm);
  hierarchical::to_file(input, input_fn);

  CHECK(tree == input);  // both trees should still be identical in memory
  CHECK(input == tree);  // both trees should still be identical in memory

  using hm3::fs::binary_identical;

  // both header files should be bitwise identical: of course they are not,
  // names differ, need a "smarter" way to compare them
  // CHECK(identical(file_name + io::session::extension(), input_fn +
  // io::session::extension()));
  // both binary files should be bitwise identical:
  CHECK(binary_identical(name(tree) + "_" + file_name + "_0",
                         name(tree) + "_" + input_fn + "_0", comm));
  
  return input;
  */
  return tree;
}

}  // namespace test
}  // namespace hm3
