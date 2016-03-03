#include <hm3/solver/utility/neighbors.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

using hierarchical::grid_node_idx;
using hierarchical::operator""_gn;

/// Explicit instantiate it
template struct hm3::solver::state::neighbors<6>;

struct node {
  grid_node_idx idx;
  std::vector<grid_node_idx> neighbors;
  node(int nidx, std::initializer_list<int> ns)
   : idx{nidx}, neighbors(ns.size()) {
    transform(ns, begin(neighbors), [](auto&& n) { return grid_node_idx{n}; });
  }
};

std::vector<node> state0{
 {0, {1, 2, 3}},           //
 {1, {0, 2, 3, 4}},        //
 {2, {0, 1, 3, 5}},        //
 {3, {0, 1, 2, 4, 5, 6}},  //
 {4, {1, 3, 5, 6}},        //
 {5, {2, 3, 4, 6}},        //
 {6, {3, 4, 5}}            //
};

std::vector<node> state_after_swap0{
 {0, {3, 1, 2, 4, 5, 6}},  //
 {1, {3, 2, 0, 4}},        //
 {2, {3, 1, 0, 5}},        //
 {3, {1, 2, 0}},           //
 {4, {1, 0, 5, 6}},        //
 {5, {2, 0, 4, 6}},        //
 {6, {0, 4, 5}}            //
};

std::vector<node> state_after_swap1{
 {0, {3, 6, 2, 4, 5, 1}},  //
 {1, {0, 4, 5}},           //
 {2, {3, 6, 0, 5}},        //
 {3, {6, 2, 0}},           //
 {4, {6, 0, 5, 1}},        //
 {5, {2, 0, 4, 1}},        //
 {6, {3, 2, 0, 4}}         //
};

std::vector<node> state_after_pop0{
 {0, {6, 2, 4, 5, 1}},  //
 {1, {0, 4, 5}},        //
 {2, {6, 0, 5}},        //
 {3, {}},               //
 {4, {6, 0, 5, 1}},     //
 {5, {2, 0, 4, 1}},     //
 {6, {2, 0, 4}}         //
};

template <typename State, typename NeighborState>
void check(State&& s, NeighborState ns) {
  for (auto n : s) {
    auto gns = ns(n.idx);
    CHECK(gns.size() == n.neighbors.size());
    for (auto i : n.neighbors) { CHECK(ns.has_neighbor(n.idx, i)); }
  }
}

int main() {
  solver::state::neighbors<6> neighbors(7);

  for (auto n : state0) {
    for (auto i : n.neighbors) {
      if (!neighbors.has_neighbor(n.idx, i)) { neighbors.push(n.idx, i); }
    }
  }
  check(state0, neighbors);

  neighbors.swap(0_gn, 3_gn);
  check(state_after_swap0, neighbors);

  neighbors.swap(1_gn, 6_gn);
  check(state_after_swap1, neighbors);

  neighbors.pop(3_gn);
  check(state_after_pop0, neighbors);

  return test::result();
}
