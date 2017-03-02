#include <hm3/geometry/data_structure/point_set.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using namespace geometry;
  using namespace tree;

  {  // 1D
    static constexpr suint_t nd = 1;

    using p_t   = point<nd>;
    using ps    = point_set<p_t>;
    using box_t = box<nd>;

    ps s;
    CHECK(s.size() == 0_u);
    CHECK(s.empty());

    p_t x0{0.};
    p_t x1{1.};
    p_t ps01[] = {p_t{0.1}, p_t{0.2}, p_t{0.3}, p_t{0.4}, p_t{0.5},
                  p_t{0.6}, p_t{0.7}, p_t{0.8}, p_t{0.9}};

    p_t ps23[] = {p_t{2.1}, p_t{2.2}, p_t{2.3}, p_t{2.4}, p_t{2.5},
                  p_t{2.6}, p_t{2.7}, p_t{2.8}, p_t{2.9}};

    // An empty point set does not contain any points
    for (auto&& p : ps01) { CHECK(not s.contains(p)); }

    // Bounding box of an empty point set
    CHECK(s.bounding_box() == box_t{});

    // Add points (test growth and rebuild of bbox)
    s.push_back(x0);

    CHECK(not s.empty());
    CHECK(s.size() == 1_u);
    CHECK(s.contains(x0));
    CHECK(not s.contains(x1));
    CHECK(s.bounding_box() == bounding_volume.box(x0));

    for (auto&& p : ps01) { CHECK(not s.contains(p)); }
    for (auto&& p : ps23) { CHECK(not s.contains(p)); }
    CHECK(not s.contains(p_t{1.001}));
    CHECK(not s.contains(p_t{-0.001}));

    s.push_back(x1);
    CHECK(s.size() == 2_u);
    CHECK(s.contains(x0));
    CHECK(s.contains(x1));
    CHECK(s.bounding_box() == box_t{x0, x1});

    for (auto&& p : ps01) { CHECK(not s.contains(p)); }
    for (auto&& p : ps23) { CHECK(not s.contains(p)); }
    CHECK(not s.contains(p_t{1.001}));
    CHECK(not s.contains(p_t{-0.001}));

    s.push_back(p_t{3.});

    CHECK(s.size() == 3_u);
    CHECK(s.bounding_box() == box_t{x0, p_t{3.}});
    for (auto&& p : ps23) { CHECK(not s.contains(p)); }

    CHECK(not s.contains(p_t{1.001}));
    CHECK(not s.contains(p_t{-0.001}));

    for (auto&& p : ps01) { s.push_back(p); }
    CHECK(s.size() == 12_u);

    for (auto&& p : ps01) { CHECK(s.contains(p)); }
    CHECK(not s.contains(p_t{1.001}));
    CHECK(not s.contains(p_t{-0.001}));
    CHECK(s.bounding_box() == box_t{x0, p_t{3.}});

    for (auto&& p : ps23) { s.push_back(p); }
    for (auto&& p : ps01) { CHECK(s.contains(p)); }
    for (auto&& p : ps23) { CHECK(s.contains(p)); }
    CHECK(s.size() == 21_u);
    CHECK(s.bounding_box() == box_t{x0, p_t{3.}});
    CHECK(not s.contains(p_t{1.001}));
    CHECK(not s.contains(p_t{-0.001}));
  }

  {  // 2D
    static constexpr suint_t nd = 2;

    using p_t = point<nd>;
    using ps  = point_set<p_t>;

    {
      ps s;
      CHECK(s.size() == 0_u);
    }

    // clang-format off
  p_t points[] = {
    {-0.9, 2.9},  // 0
    {2.9, -0.9},  // 1
    {2.9, 2.9}, // 2
    {-0.9, -0.9}, // 3
    {0.9, 0.9}, // 4
    {1.1, 1.1}, // 5
    {0.1, 0.1}, // 6
    {0.9, 0.1}  // 7
   //
  };
  p_t points_not_in_set[] = {
    {2., 0.},  // 0
    {0., 2.},  // 1
    //
  };
    // clang-format on

    box<nd> bbox(p_t::constant(1.), 4);

    ps s(40, bbox);  // 20 to trigger assertion

    for (int k = 0; k < 100; ++k) {
      for (suint_t i = 0; i < size(points); ++i) {
        CHECK(s.push_back(points[i]) == i);
        CHECK(s.contains(points[i]).value() == i);
      }
      CHECK(s.size() == size(points));
      for (suint_t i = 0; i < size(points_not_in_set); ++i) {
        CHECK(!s.contains(points_not_in_set[i]));
      }
    }

    CHECK(s.bounding_box() == bbox);
  }

  return test::result();
}
