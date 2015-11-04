/// \file
///
/// TODO
/// - move to solver/utility/structured_grid.cpp
/// - test external iterators
/// - test from
/// - test at
/// - test is_halo , test is_internal
/// - test internal iterators
/// - test internal interators for internal cells with halo layers offset h
#include <hm3/solver/utility/structured_grid.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template struct hm3::solver::square_structured_indices<1, 2, 2>;
template struct hm3::solver::square_structured_indices<2, 2, 2>;
template struct hm3::solver::square_structured_indices<3, 2, 2>;

int main() {
  using namespace solver;

  uint_t rm[27] = {
   // k = 0
   0, 1, 2,  //
   3, 4, 5,  //
   6, 7, 8,  //
   // k = 1
   9, 10, 11,   //
   12, 13, 14,  //
   15, 16, 17,  //
   // k = 2
   18, 19, 20,  //
   21, 22, 23,  //
   24, 25, 26,  //
  };

  {  // 1D:
    using indices_t = square_structured_indices<1, 23, 2>;
    using c         = typename indices_t::coordinates_t;

    constexpr indices_t b{};

    CHECK(b.halo_layers() == 2_u);
    CHECK(b.internal_cells_per_length() == 23_u);
    CHECK(b.cells_per_length() == 27_u);

    for (uint_t i = 0; i != 27; ++i) {
      CHECK(rm[b.from(c{{i}}).idx] == i);
      CHECK(b.from(b.from(c{{i}}).idx).x == c{{i}});
    }

    auto* p = &rm[2];
    b.for_each_internal([&](auto i) {
      CHECK(i.idx == *p);
      ++p;
    });
    CHECK(*p == 25_u);

    p = &rm[1];
    b.for_each_internal(
     [&](auto i) {
       CHECK(i.idx == *p);
       CHECK(b.at(i.idx, 0, -1).idx == (*p) - 1);
       CHECK(b.at(i.idx, 0, 1).idx == (*p) + 1);
       ++p;
     },
     1);
    CHECK(*p == 26_u);

    uint_t halos[] = {0, 1, 25, 26};
    uint_t count_  = 0;
    b.for_each_halo([&](auto i) {
      CHECK(i.idx == halos[count_]);
      ++count_;
    });
    CHECK(count_ == 4_u);
  }

  {  // 2D:
    using indices_t = square_structured_indices<2, 1, 2>;
    using c         = typename indices_t::coordinates_t;

    constexpr indices_t b{};

    CHECK(b.halo_layers() == 2_u);
    CHECK(b.internal_cells_per_length() == 1_u);
    CHECK(b.cells_per_length() == 5_u);

    uint_t count_ = 0;
    for (uint_t j = 0; j != 5; ++j) {
      for (uint_t i = 0; i != 5; ++i) {
        CHECK(rm[b.from(c{{i, j}}).idx] == count_);
        CHECK(b.from(b.from(c{{i, j}}).idx).x == c{{i, j}});
        ++count_;
      }
    }

    CHECK(count_ == 25_u);

    count_ = 0;
    b.for_each_internal([&](auto i) {
      CHECK(i.idx == 12_u);
      ++count_;
    });
    CHECK(count_ == 1_u);

    uint_t internal1[] = {6, 7, 8, 11, 12, 13, 16, 17, 18};
    count_             = 0;
    b.for_each_internal(
     [&](auto i) {
       auto expected = internal1[count_];
       CHECK(i.idx == expected);
       CHECK(b.at(i.idx, 0, -1).idx == expected - 1);
       CHECK(b.at(i.idx, 0, 1).idx == expected + 1);
       CHECK(b.at(i.idx, 1, -1).idx == expected - b.cells_per_length());
       CHECK(b.at(i.idx, 1, 1).idx == expected + b.cells_per_length());

       ++count_;
     },
     1);
    CHECK(count_ == 9_u);

    uint_t halos[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    count_ = 0;
    b.for_each_halo([&](auto i) {
      CHECK(i.idx == halos[count_]);
      ++count_;
    });
    CHECK(count_ == 24_u);
  }

  {  // 2D:
    using indices_t = square_structured_indices<2, 2, 2>;
    using c         = typename indices_t::coordinates_t;

    constexpr indices_t b{};

    CHECK(b.halo_layers() == 2_u);
    CHECK(b.internal_cells_per_length() == 2_u);
    CHECK(b.cells_per_length() == 6_u);

    auto rm = view::iota(0_u, 36_u);

    uint_t count_ = 0;
    for (uint_t j = 0; j != 6; ++j) {
      for (uint_t i = 0; i != 6; ++i) {
        CHECK(rm[b.from(c{{i, j}}).idx] == count_);
        CHECK(b.from(b.from(c{{i, j}}).idx).x == c{{i, j}});
        ++count_;
      }
    }

    CHECK(count_ == 36_u);

    uint_t internal[] = {14, 15, 20, 21};
    count_            = 0;
    b.for_each_internal([&](auto i) {
      CHECK(i.idx == internal[count_]);
      ++count_;
    });
    CHECK(count_ == 4_u);

    uint_t internal1[]
     = {7, 8, 9, 10, 13, 14, 15, 16, 19, 20, 21, 22, 25, 26, 27, 28};
    count_ = 0;
    b.for_each_internal(
     [&](auto i) {
       auto expected = internal1[count_];
       CHECK(i.idx == expected);
       CHECK(b.at(i.idx, 0, -1).idx == expected - 1);
       CHECK(b.at(i.idx, 0, 1).idx == expected + 1);
       CHECK(b.at(i.idx, 1, -1).idx == expected - b.cells_per_length());
       CHECK(b.at(i.idx, 1, 1).idx == expected + b.cells_per_length());
       ++count_;
     },
     1);
    CHECK(count_ == 16_u);

    uint_t halos[]
     = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 16, 17,
        18, 19, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    count_ = 0;
    b.for_each_halo([&](auto i) {
      CHECK(i.idx == halos[count_]);
      ++count_;
    });
    CHECK(count_ == 32_u);
  }

  {  // 3D
    using indices_t = square_structured_indices<3, 1, 1>;
    using c         = typename indices_t::coordinates_t;

    constexpr indices_t b{};

    CHECK(b.halo_layers() == 1_u);
    CHECK(b.internal_cells_per_length() == 1_u);
    CHECK(b.cells_per_length() == 3_u);

    uint_t count_ = 0;
    for (uint_t k = 0; k != 3; ++k) {
      for (uint_t j = 0; j != 3; ++j) {
        for (uint_t i = 0; i != 3; ++i) {
          CHECK(rm[b.from(c{{i, j, k}}).idx] == count_);
          CHECK(b.from(b.from(c{{i, j, k}}).idx).x == c{{i, j, k}});
          ++count_;
        }
      }
    }
  }
  return 0;
}
