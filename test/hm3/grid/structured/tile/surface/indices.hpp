#include <hm3/geometry/point.hpp>
#include <hm3/grid/structured/tile/surface/indices.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/test.hpp>

namespace hm3 {

struct test_1d_5 {
  static auto normal(suint_t) { return geometry::point<1>::constant(1.); }
  using opt_i = optional<suint_t>;
  static opt_i right(suint_t i) { return i < 5 ? opt_i{i} : opt_i{}; }
  static opt_i left(suint_t i) { return i > 0 ? opt_i{i - 1} : opt_i{}; }
  static constexpr suint_t size() noexcept { return 6; }
};

struct test_2d_3 {
  static constexpr suint_t size() noexcept { return 24; }
  static auto normal(suint_t i) {
    return geometry::point<2>::unit(i <= 11 ? 0 : 1);
  }
  using opt_i = optional<suint_t>;

  static auto lr(suint_t i) {
    return hm3::array<hm3::array<opt_i, 2>, 24>{{
     //
     {{opt_i(), 0}},  // 0
     {{0, 1}},        // 1
     {{1, 2}},        // 2
     {{2, opt_i()}},  // 3
     //
     {{opt_i(), 3}},  // 4
     {{3, 4}},        // 5
     {{4, 5}},        // 6
     {{5, opt_i()}},  // 7
     //
     {{opt_i(), 6}},  // 8
     {{6, 7}},        // 9
     {{7, 8}},        // 10
     {{8, opt_i()}},  // 11
                      //
     //
     {{opt_i(), 0}},  // 12
     {{0, 3}},        // 13
     {{3, 6}},        // 14
     {{6, opt_i()}},  // 15
     //
     {{opt_i(), 1}},  // 16
     {{1, 4}},        // 17
     {{4, 7}},        // 18
     {{7, opt_i()}},  // 19
     //
     {{opt_i(), 2}},  // 20
     {{2, 5}},        // 21
     {{5, 8}},        // 22
     {{8, opt_i()}},  // 23
                      //
    }}[i];
  }
  static opt_i right(suint_t i) { return lr(i)[1]; }
  static opt_i left(suint_t i) { return lr(i)[0]; }
};

struct test_3d_2 {
  static constexpr suint_t size() noexcept { return 36; }
  static auto normal(suint_t i) {
    if (i <= 11) { return geometry::point<3>::unit(0); }
    if (i <= 23) { return geometry::point<3>::unit(1); }
    return geometry::point<3>::unit(2);
  }
  using opt_i = std2::experimental::optional<suint_t>;

  static auto lr(suint_t i) {
    return hm3::array<hm3::array<opt_i, 2>, 36>{{
     //
     {{opt_i(), 0}},  // 0
     {{0, 1}},        // 1
     {{1, opt_i()}},  // 2
     //
     {{opt_i(), 2}},  // 3
     {{2, 3}},        // 4
     {{3, opt_i()}},  // 5
                      //
     {{opt_i(), 4}},  // 6
     {{4, 5}},        // 7
     {{5, opt_i()}},  // 8
     //
     {{opt_i(), 6}},  // 9
     {{6, 7}},        // 10
     {{7, opt_i()}},  // 11

     //
     {{opt_i(), 0}},  // 12
     {{0, 2}},        // 13
     {{2, opt_i()}},  // 14
                      //
     {{opt_i(), 1}},  // 15
     {{1, 3}},        // 16
     {{3, opt_i()}},  // 17
                      //
     {{opt_i(), 4}},  // 18
     {{4, 6}},        // 19
     {{6, opt_i()}},  // 20
                      //
     {{opt_i(), 5}},  // 21
     {{5, 7}},        // 22
     {{7, opt_i()}},  // 23

     //
     {{opt_i(), 0}},  // 24
     {{0, 4}},        // 25
     {{4, opt_i()}},  // 26
                      //
     {{opt_i(), 1}},  // 27
     {{1, 5}},        // 28
     {{5, opt_i()}},  // 29
                      //
     {{opt_i(), 2}},  // 30
     {{2, 6}},        // 31
     {{6, opt_i()}},  // 32
                      //
     {{opt_i(), 3}},  // 33
     {{3, 7}},        // 34
     {{7, opt_i()}},  // 35

     //
    }}[i];
  }
  static opt_i right(suint_t i) { return lr(i)[1]; }
  static opt_i left(suint_t i) { return lr(i)[0]; }
};

template <typename Indices, typename Should>
void check_surface_indices(Indices, Should) {
  suint_t idx_should = 0;
  constexpr Indices ids{};
  using x_s = typename Indices::coordinate;
  using x_c = typename Indices::cell_coordinate;
  using ci  = typename Indices::cell_indices;
  std::vector<x_s> surfaces;
  surfaces.reserve(ids.size());
  ids.for_each([&](auto&& x_s) {
    surfaces.push_back(x_s);
    using x_t              = uncvref_t<decltype(x_s)>;
    auto left_cell_should  = Should::left(idx_should);
    auto right_cell_should = Should::right(idx_should);
    auto normal_should     = Should::normal(idx_should);

    auto normal     = x_s.normal();
    auto left_cell  = x_s.adjacent_neg();
    auto right_cell = x_s.adjacent_pos();
    auto idx        = *x_s.idx();
    auto x_srfc     = x_t(idx);

    // check global surface index
    CHECK(idx == idx_should);
    CHECK(x_srfc.idx() == idx);
    CHECK(x_srfc == x_s);

    CHECK(normal == normal_should);

    // check left and right cells of a surface
    if (left_cell_should) {
      CHECK(left_cell);
      CHECK(left_cell.idx() == *left_cell_should);
    } else {
      CHECK(!left_cell);
    }
    if (right_cell_should) {
      CHECK(right_cell);
      CHECK(right_cell.idx() == *right_cell_should);
    } else {
      CHECK(!right_cell);
    }

    // check cell surfaces
    if (right_cell_should) {
      suint_t cell_surface_count = 0;
      ids.for_each(right_cell, [&](auto&& x_c_s) {
        auto x_c_r = x_c_s.adjacent_pos();
        auto x_c_l = x_c_s.adjacent_neg();

        if (cell_surface_count % 2 == 0) {
          CHECK(x_c_r);
          CHECK(x_c_r == right_cell);
        } else {
          CHECK(x_c_l);
          CHECK(x_c_l == right_cell);
        }
        ++cell_surface_count;
      });
      CHECK(cell_surface_count == Indices::size_per_cell());
    }

    ++idx_should;
  });
  CHECK(idx_should == Should::size());

  suint_t idx = 0;
  ids.for_each(x_c::constant(0), x_c::constant(ci::length() - 1),
               [&](auto&& x_s) {
                 CHECK(x_s == surfaces[idx]);
                 ++idx;
               });
  CHECK(idx == Should::size());
  CHECK(idx == surfaces.size());
}

}  // namespace hm3
