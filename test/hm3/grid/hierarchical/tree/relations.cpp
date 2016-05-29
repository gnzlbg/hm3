/// \file
///
/// Tree relations tests
#include "tree.hpp"

using namespace hm3;
using namespace test;

int main() {
  // Test no children:
  STATIC_CHECK(no_children(1) == 2_su);
  STATIC_CHECK(no_children(2) == 4_su);
  STATIC_CHECK(no_children(3) == 8_su);

  // Test no siblings
  STATIC_CHECK(no_siblings(1) == 2_su);
  STATIC_CHECK(no_siblings(2) == 4_su);
  STATIC_CHECK(no_siblings(3) == 8_su);

  // Test no nodes sharing face: 0D
  STATIC_CHECK(no_nodes_sharing_face(0, 0) == 1_su);
  STATIC_CHECK(no_nodes_sharing_face(0, 1) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(0, 2) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(0, 3) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(0, 4) == 0_su);

  // Test no nodes sharing face: 1D
  STATIC_CHECK(no_nodes_sharing_face(1, 0) == 1_su);
  STATIC_CHECK(no_nodes_sharing_face(1, 1) == 2_su);
  STATIC_CHECK(no_nodes_sharing_face(1, 2) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(1, 3) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(1, 4) == 0_su);

  // Test no nodes sharing face: 2D
  STATIC_CHECK(no_nodes_sharing_face(2, 0) == 1_su);
  STATIC_CHECK(no_nodes_sharing_face(2, 1) == 2_su);
  STATIC_CHECK(no_nodes_sharing_face(2, 2) == 4_su);
  STATIC_CHECK(no_nodes_sharing_face(2, 3) == 0_su);
  STATIC_CHECK(no_nodes_sharing_face(2, 4) == 0_su);

  // Test no nodes sharing face: 3D
  STATIC_CHECK(no_nodes_sharing_face(3, 0) == 1_su);
  STATIC_CHECK(no_nodes_sharing_face(3, 1) == 2_su);
  STATIC_CHECK(no_nodes_sharing_face(3, 2) == 4_su);
  STATIC_CHECK(no_nodes_sharing_face(3, 3) == 8_su);
  STATIC_CHECK(no_nodes_sharing_face(3, 4) == 0_su);

  // Test no nodes sharing face at level: 0D
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 0, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 0, 1) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 0, 2) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 1, 0) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 1, 1) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(0, 1, 2) == 0_u);

  // Test no nodes sharing face at level: 1D
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 0, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 0, 1) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 0, 2) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 1, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 1, 1) == 2_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 1, 2) == 4_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 2, 0) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 2, 1) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(1, 2, 2) == 0_u);

  // Test no nodes sharing face at level: 2D
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 0, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 0, 1) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 0, 2) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 1, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 1, 1) == 2_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 1, 2) == 4_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 1, 3) == 8_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 2, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 2, 1) == 4_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 2, 2) == 16_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 3, 0) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 3, 1) == 0_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(2, 3, 2) == 0_u);

  // Test no nodes sharing face at level: 3D
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 0, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 0, 1) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 0, 2) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 1, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 1, 1) == 2_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 1, 2) == 4_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 1, 3) == 8_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 2, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 2, 1) == 4_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 2, 2) == 16_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 3, 0) == 1_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 3, 1) == 8_u);
  STATIC_CHECK(no_nodes_sharing_face_at_level(3, 3, 2) == 64_u);

  /// Test number of node faces: 1D
  STATIC_CHECK(no_faces(1, 0) == 2_su);
  STATIC_CHECK(no_faces(1, 1) == 1_su);
  STATIC_CHECK(no_faces(1, 2) == 0_su);

  /// Test number of node faces: 2D
  STATIC_CHECK(no_faces(2, 0) == 4_su);
  STATIC_CHECK(no_faces(2, 1) == 4_su);
  STATIC_CHECK(no_faces(2, 2) == 1_su);

  /// Test number of node faces: 3D
  STATIC_CHECK(no_faces(3, 0) == 8_su);
  STATIC_CHECK(no_faces(3, 1) == 12_su);
  STATIC_CHECK(no_faces(3, 2) == 6_su);
  STATIC_CHECK(no_faces(3, 3) == 1_su);

  /// Check face neighbors: 1D
  {
    constexpr auto fn = face_neighbors<1>{};
    STATIC_CHECK(fn.size() == 2_su);
    CHECK(size(fn()) == 2_u);
    test::check_equal(fn[0], neighbor_offset<1>{{-1}});
    test::check_equal(fn[1], neighbor_offset<1>{{1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<1, 0>);
  }

  /// Check face neighbors: 2D
  {
    constexpr auto fn = face_neighbors<2>{};
    STATIC_CHECK(fn.size() == 4_su);
    CHECK(size(fn()) == 4_u);
    test::check_equal(fn[0], neighbor_offset<2>{{-1, 0}});
    test::check_equal(fn[1], neighbor_offset<2>{{1, 0}});
    test::check_equal(fn[2], neighbor_offset<2>{{0, -1}});
    test::check_equal(fn[3], neighbor_offset<2>{{0, 1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<2, 1>);
  }

  /// Check face neighbors: 3D
  {
    constexpr auto fn = face_neighbors<3>{};
    STATIC_CHECK(fn.size() == 6_su);
    CHECK(size(fn()) == 6_u);
    test::check_equal(fn[0], neighbor_offset<3>{{-1, 0, 0}});
    test::check_equal(fn[1], neighbor_offset<3>{{1, 0, 0}});
    test::check_equal(fn[2], neighbor_offset<3>{{0, -1, 0}});
    test::check_equal(fn[3], neighbor_offset<3>{{0, 1, 0}});
    test::check_equal(fn[4], neighbor_offset<3>{{0, 0, -1}});
    test::check_equal(fn[5], neighbor_offset<3>{{0, 0, 1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<3, 2>);
  }

  /// Check edge neighbors: 1D
  {
    constexpr auto fn = edge_neighbors<1>{};
    STATIC_CHECK(fn.size() == 0_su);
    CHECK(size(fn()) == 0_u);
    auto o = fn.offsets();
    auto m = neighbor_lookup_table<1, 100>;
    test::check_equal(o, m);
  }

  /// Check edge neighbors: 2D
  {
    constexpr auto fn = edge_neighbors<2>{};
    STATIC_CHECK(fn.size() == 4_su);
    CHECK(size(fn()) == 4_u);
    test::check_equal(fn[0], neighbor_offset<2>{{-1, -1}});
    test::check_equal(fn[1], neighbor_offset<2>{{1, -1}});
    test::check_equal(fn[2], neighbor_offset<2>{{-1, 1}});
    test::check_equal(fn[3], neighbor_offset<2>{{1, 1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<2, 0>);
  }

  /// Check edge neighbors: 3D
  {
    constexpr auto fn = edge_neighbors<3>{};
    STATIC_CHECK(fn.size() == 12_su);
    CHECK(size(fn()) == 12_u);

    test::check_equal(fn[0], neighbor_offset<3>{{-1, -1, 0}});
    test::check_equal(fn[1], neighbor_offset<3>{{1, -1, 0}});
    test::check_equal(fn[2], neighbor_offset<3>{{-1, 1, 0}});
    test::check_equal(fn[3], neighbor_offset<3>{{1, 1, 0}});

    test::check_equal(fn[4], neighbor_offset<3>{{-1, 0, -1}});
    test::check_equal(fn[5], neighbor_offset<3>{{1, 0, -1}});
    test::check_equal(fn[6], neighbor_offset<3>{{0, -1, -1}});
    test::check_equal(fn[7], neighbor_offset<3>{{0, 1, -1}});

    test::check_equal(fn[8], neighbor_offset<3>{{-1, 0, 1}});
    test::check_equal(fn[9], neighbor_offset<3>{{1, 0, 1}});
    test::check_equal(fn[10], neighbor_offset<3>{{0, -1, 1}});
    test::check_equal(fn[11], neighbor_offset<3>{{0, 1, 1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<3, 1>);
  }

  /// Check corner neighbors: 1D
  {
    constexpr auto fn = corner_neighbors<1>{};
    STATIC_CHECK(fn.size() == 0_su);
    CHECK(size(fn()) == 0_u);
    test::check_equal(fn.offsets(), neighbor_lookup_table<1, 100>);
  }

  /// Check corner neighbors: 2D
  {
    constexpr auto fn = corner_neighbors<2>{};
    STATIC_CHECK(fn.size() == 0_su);
    CHECK(size(fn()) == 0_u);
    test::check_equal(fn.offsets(), neighbor_lookup_table<2, 100>);
  }

  /// Check corner neighbors: 3D
  {
    constexpr auto fn = corner_neighbors<3>{};
    STATIC_CHECK(fn.size() == 8_su);
    CHECK(size(fn()) == 8_u);
    test::check_equal(fn[0], neighbor_offset<3>{{-1, -1, -1}});
    test::check_equal(fn[1], neighbor_offset<3>{{1, -1, -1}});
    test::check_equal(fn[2], neighbor_offset<3>{{-1, 1, -1}});
    test::check_equal(fn[3], neighbor_offset<3>{{1, 1, -1}});
    test::check_equal(fn[4], neighbor_offset<3>{{-1, -1, 1}});
    test::check_equal(fn[5], neighbor_offset<3>{{1, -1, 1}});
    test::check_equal(fn[6], neighbor_offset<3>{{-1, 1, 1}});
    test::check_equal(fn[7], neighbor_offset<3>{{1, 1, 1}});
    test::check_equal(fn.offsets(), neighbor_lookup_table<3, 0>);
  }

  /// Check all neighbors: 1D
  {
    int manifold_count = 0;
    for_each_neighbor_manifold<1>([&](auto m) {
      CHECK(m.rank() == 1_u);
      CHECK(m.size() == 2_u);
      ++manifold_count;
    });
    CHECK(manifold_count == 1);
  }

  /// Check all neighbors: 2D
  {
    int manifold_count = 0;
    for_each_neighbor_manifold<2>([&](auto m) {
      if (m.rank() == 1_u) {
        CHECK(m.size() == 4_u);
      } else if (m.rank() == 2_u) {
        CHECK(m.size() == 4_u);
      } else {
        CHECK(false);
      }
      ++manifold_count;
    });
    CHECK(manifold_count == 2);
  }

  /// Check all neighbors: 3D
  {
    int manifold_count = 0;
    for_each_neighbor_manifold<3>([&](auto m) {
      if (m.rank() == 1_u) {
        CHECK(m.size() == 6_u);
      } else if (m.rank() == 2_u) {
        CHECK(m.size() == 12_u);
      } else if (m.rank() == 3_u) {
        CHECK(m.size() == 8_u);
      } else {
        CHECK(false);
      }
      ++manifold_count;
    });
    CHECK(manifold_count == 3);
  }

  /// Test max no neighbors
  {
    STATIC_CHECK(max_no_neighbors(1) == 2_su);
    STATIC_CHECK(max_no_neighbors(2) == 12_su);
    STATIC_CHECK(max_no_neighbors(3) == 56_su);
  }

  /// Test opposite neighbor stencils
  {
    auto test_opposite_neighbor_positions = [](auto neighbors, auto stencil) {
      std::size_t c = 0;
      CHECK(size(neighbors()) == size(stencil));
      for (auto p : neighbors()) {
        CHECK(opposite(p) == stencil[c]);
        c++;
      }
    };
    // faces:
    test_opposite_neighbor_positions(face_neighbors<1>{},
                                     hm3::array<uint_t, 2>{{1, 0}});
    test_opposite_neighbor_positions(face_neighbors<2>{},
                                     hm3::array<uint_t, 4>{{1, 0, 3, 2}});
    test_opposite_neighbor_positions(face_neighbors<3>{},
                                     hm3::array<uint_t, 6>{{1, 0, 3, 2, 5, 4}});

    // edges:
    test_opposite_neighbor_positions(edge_neighbors<1>{},
                                     hm3::array<uint_t, 0>{});
    test_opposite_neighbor_positions(edge_neighbors<2>{},
                                     hm3::array<uint_t, 4>{{3, 2, 1, 0}});
    test_opposite_neighbor_positions(
     edge_neighbors<3>{},
     hm3::array<uint_t, 12>{{3, 2, 1, 0, 9, 8, 11, 10, 5, 4, 7, 6}});

    // corners:
    test_opposite_neighbor_positions(corner_neighbors<1>{},
                                     hm3::array<uint_t, 0>{});
    test_opposite_neighbor_positions(corner_neighbors<2>{},
                                     hm3::array<uint_t, 0>{{}});
    test_opposite_neighbor_positions(
     corner_neighbors<3>{}, hm3::array<uint_t, 8>{{7, 6, 5, 4, 3, 2, 1, 0}});
  }

  /// No nodes at/until uniform level
  {
    STATIC_CHECK(no_nodes_at_uniform_level(1, 0) == 1_u);
    STATIC_CHECK(no_nodes_until_uniform_level(1, 0) == 1_u);
    STATIC_CHECK(no_nodes_at_uniform_level(1, 1) == 2_u);
    STATIC_CHECK(no_nodes_until_uniform_level(1, 1) == 3_u);
    STATIC_CHECK(no_nodes_at_uniform_level(1, 2) == 4_u);
    STATIC_CHECK(no_nodes_until_uniform_level(1, 2) == 7_u);
    STATIC_CHECK(no_nodes_at_uniform_level(1, 3) == 8_u);
    STATIC_CHECK(no_nodes_until_uniform_level(1, 3) == 15_u);

    STATIC_CHECK(no_nodes_at_uniform_level(2, 0) == 1_u);
    STATIC_CHECK(no_nodes_until_uniform_level(2, 0) == 1_u);
    STATIC_CHECK(no_nodes_at_uniform_level(2, 1) == 4_u);
    STATIC_CHECK(no_nodes_until_uniform_level(2, 1) == 5_u);
    STATIC_CHECK(no_nodes_at_uniform_level(2, 2) == 16_u);
    STATIC_CHECK(no_nodes_until_uniform_level(2, 2) == 21_u);
    STATIC_CHECK(no_nodes_at_uniform_level(2, 3) == 64_u);
    STATIC_CHECK(no_nodes_until_uniform_level(2, 3) == 85_u);

    STATIC_CHECK(no_nodes_at_uniform_level(3, 0) == 1_u);
    STATIC_CHECK(no_nodes_until_uniform_level(3, 0) == 1_u);
    STATIC_CHECK(no_nodes_at_uniform_level(3, 1) == 8_u);
    STATIC_CHECK(no_nodes_until_uniform_level(3, 1) == 9_u);
    STATIC_CHECK(no_nodes_at_uniform_level(3, 2) == 64_u);
    STATIC_CHECK(no_nodes_until_uniform_level(3, 2) == 73_u);
  }

  {  // Node length at level
    STATIC_CHECK(node_length_at_level(0) == 1.);
    STATIC_CHECK(node_length_at_level(1) == 0.5);
    STATIC_CHECK(node_length_at_level(2) == 0.25);
    STATIC_CHECK(node_length_at_level(3) == 0.125);
  }

  /// Relative child positions
  {
    // child 0
    STATIC_CHECK(relative_child_position<3>(0)[0] == -1);
    STATIC_CHECK(relative_child_position<3>(0)[1] == -1);
    STATIC_CHECK(relative_child_position<3>(0)[2] == -1);
    // child 1
    STATIC_CHECK(relative_child_position<3>(1)[0] == 1);
    STATIC_CHECK(relative_child_position<3>(1)[1] == -1);
    STATIC_CHECK(relative_child_position<3>(1)[2] == -1);
    // child 2
    STATIC_CHECK(relative_child_position<3>(2)[0] == -1);
    STATIC_CHECK(relative_child_position<3>(2)[1] == 1);
    STATIC_CHECK(relative_child_position<3>(2)[2] == -1);
    // child 3
    STATIC_CHECK(relative_child_position<3>(3)[0] == 1);
    STATIC_CHECK(relative_child_position<3>(3)[1] == 1);
    STATIC_CHECK(relative_child_position<3>(3)[2] == -1);
    // child 4
    STATIC_CHECK(relative_child_position<3>(4)[0] == -1);
    STATIC_CHECK(relative_child_position<3>(4)[1] == -1);
    STATIC_CHECK(relative_child_position<3>(4)[2] == 1);
    // child 5
    STATIC_CHECK(relative_child_position<3>(5)[0] == 1);
    STATIC_CHECK(relative_child_position<3>(5)[1] == -1);
    STATIC_CHECK(relative_child_position<3>(5)[2] == 1);
    // child 6
    STATIC_CHECK(relative_child_position<3>(6)[0] == -1);
    STATIC_CHECK(relative_child_position<3>(6)[1] == 1);
    STATIC_CHECK(relative_child_position<3>(6)[2] == 1);
    // child 7
    STATIC_CHECK(relative_child_position<3>(7)[0] == 1);
    STATIC_CHECK(relative_child_position<3>(7)[1] == 1);
    STATIC_CHECK(relative_child_position<3>(7)[2] == 1);
  }

  return test::result();
}
