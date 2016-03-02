#include <hm3/structured/square/tile.hpp>
#include <hm3/utility/test.hpp>

/// Enables HM3_BENCHMARKS
//#define HM3_ENABLE_BENCHMARKS

using namespace hm3;

template <typename I, typename X>
void coordinate_tests(I index, suint_t length, const X x) {
  using namespace hm3::structured::square;
  {
    X a;
    CHECK(!a);
  }
  X y(index);
  CHECK(y.idx() == index);
  CHECK(I(y) == index);
  CHECK(x == y);
  CHECK(!(x != y));
  CHECK(x <= y);
  CHECK(x >= y);

  if (index < length - 1) {
    X z(++index);
    CHECK(x != z);
    CHECK(!(x == z));
    CHECK(x < z);
    CHECK(x <= z);
    CHECK(z > x);
    CHECK(z >= x);
  }

  // Single offset +1
  for (auto&& d : X::dimensions()) {
    const auto w = x.offset(d, 1);
    if (x[d] < length - 1) {
      CHECK(w);
      CHECK(tile::distance(x, w) == 1.);
      CHECK(tile::distance(x, w) == tile::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Single offset -1
  for (auto&& d : X::dimensions()) {
    auto w = x.offset(d, -1);
    if (x[d] > 0) {
      CHECK(w);
      CHECK(tile::distance(x, w) == 1.);
      CHECK(tile::distance(x, w) == tile::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Array offset +1
  for (auto&& d : X::dimensions()) {
    std::array<sint_t, X::dimension()> o;
    o.fill(0);
    o[d]   = 1;
    auto w = x.offset(o);
    if (x[d] < length - 1) {
      CHECK(w);
      CHECK(tile::distance(x, w) == 1.);
      CHECK(tile::distance(x, w) == tile::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Array offset -1
  for (auto&& d : X::dimensions()) {
    std::array<sint_t, X::dimension()> o;
    o.fill(0);
    o[d]   = -1;
    auto w = x.offset(o);
    if (x[d] > 0) {
      CHECK(w);
      CHECK(tile::distance(x, w) == 1.);
      CHECK(tile::distance(x, w) == tile::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }
}

// find all cells from each cell
template <typename Tile>  //
void closest_cell_tests() {
  using x_t = typename Tile::coordinate;
  for (auto c : Tile::all()) {
    for (auto o : Tile::all()) {
      if (c == o) { continue; }
      int cells_visited_upper_bound
       = math::ipow(2 * accumulate(x_t::dimensions(), suint_t{1},
                                   [&](auto&& acc, auto&& i) {
                                     return std::max(
                                      acc, math::absdiff(x_t(c)[i], x_t(o)[i]));
                                   })
                     + 1,
                    (suint_t)x_t::dimension());
      int cells_visited = 0;
      auto x_c = Tile::closest_cell(x_t(c), [=, &cells_visited](auto i) {
        cells_visited++;
        return *i == suint_t{o};
      });
      if (!x_c) {
        hm3::fmt::print("Closest cell not found. Start: {}, Sought: {}\n", c,
                        o);
      }
      CHECK(x_c);
      CHECK(*x_c == suint_t{o});
      if (cells_visited > cells_visited_upper_bound) {
        hm3::fmt::print("c: {}, o: {}, cv: {}, ub: {}\n", x_t(c), x_t(o),
                        cells_visited, cells_visited_upper_bound);
      }
      CHECK(cells_visited <= cells_visited_upper_bound);
    }
  }
}

void tile_tests() {
  {  // 1x1 tile
    using tile_t = structured::square::tile::indices<1, 1>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};
    static_assert(t.size() == 1, "");
    static_assert(t.length() == 1, "");

    suint_t c      = 0;
    auto check_all = [&](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == c);
      coordinate_tests(i.idx, t.length(), i.x);
      ++c;
    };
    t.for_each(check_all);
    CHECK(c == t.size());

    c = 0_u;

    auto check = [&c](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i == *i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == c);
      ++c;
    };
    auto x_min = x_t(0);
    auto x_max = x_t(0);
    t.for_each(x_min, x_max, check);
    CHECK(c == t.size());

    c = 0_u;
    RANGES_FOR (auto i, t.sub_tile(x_min, x_max)) { check(i); }
    CHECK(c == t.size());

    closest_cell_tests<tile_t>();
  }

  {
    // 1x10 tile
    using tile_t = structured::square::tile::indices<1, 10>;
    using x_t    = typename tile_t::coordinate;

    auto f = x_t(4);
    auto l = x_t(8);

    suint_t one[]   = {4, 8};
    int counter_one = 0;
    auto check_one  = [&](auto i) {
      CHECK(*i == one[counter_one]);
      ++counter_one;
    };
    tile_t::for_each_ring(f, l, check_one, 0);
    CHECK(counter_one == 2);

    suint_t two[]   = {4, 5, 7, 8};
    int counter_two = 0;
    auto check_two  = [&](auto i) {
      CHECK(*i == two[counter_two]);
      ++counter_two;
    };
    tile_t::for_each_ring(f, l, check_two, 1);
    CHECK(counter_two == 4);

    closest_cell_tests<tile_t>();
  }
  {  // 4x4 tile
    using tile_t = structured::square::tile::indices<2, 4>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};
    static_assert(t.size() == 16, "");
    static_assert(t.length() == 4, "");

    suint_t c   = 0;
    suint_t x_i = 0;
    suint_t x_j = 0;

    auto check_all = [&](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == x_i);
      CHECK(i.x[1] == x_j);
      ++c;
      if (c % t.length() == 0) {
        x_i = 0_u;
        ++x_j;
      } else {
        ++x_i;
      }
      coordinate_tests(i.idx, t.length(), i.x);
    };
    t.for_each(check_all);
    CHECK(c == t.size());
    CHECK(x_i == 0_u);
    CHECK(x_j == t.length());

    c   = 0_u;
    x_i = 0_u;
    x_j = 0_u;
    RANGES_FOR (auto i, t.sub_tile(x_t(0, 0), x_t(3, 3))) { check_all(i); }
    CHECK(c == t.size());
    CHECK(x_i == 0_u);
    CHECK(x_j == t.length());

    c          = 5_u;
    x_i        = 1_u;
    x_j        = 1_u;
    auto check = [&](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == x_i);
      CHECK(i.x[1] == x_j);

      if (x_i == 2_u) {
        x_i = 1;
        ++x_j;
        c += 3;
      } else {
        ++x_i;
        ++c;
      }
    };
    t.for_each(x_t(1, 1), x_t(2, 2), check);

    CHECK(c == 13_u);
    CHECK(x_i == 1_u);
    CHECK(x_j == 3_u);

    c   = 5_u;
    x_i = 1_u;
    x_j = 1_u;

    RANGES_FOR (auto i, t.sub_tile(x_t(1, 1), x_t(2, 2))) { check(i); }
    CHECK(c == 13_u);
    CHECK(x_i == 1_u);
    CHECK(x_j == 3_u);

    {  // rings one:
      auto f = x_t(0, 0);
      auto l = x_t(2, 2);

      suint_t one[]   = {0, 1, 2, 4, 6, 8, 9, 10};
      int counter_one = 0;
      auto check_one  = [&](auto i) {
        CHECK(*i == one[counter_one]);
        ++counter_one;
      };
      tile_t::for_each_ring(f, l, check_one, 0);
      CHECK(counter_one == 8);
    }

    {  // rings two:
      auto f = x_t(0, 0);
      auto l = x_t(3, 3);

      suint_t counter_one = 0;
      auto check_one      = [&](auto i) {
        CHECK(*i == counter_one);
        ++counter_one;
      };
      tile_t::for_each_ring(f, l, check_one, 1);
      CHECK(counter_one == 16);
    }

    closest_cell_tests<tile_t>();
  }

  {  // 5x5x5 tile
    using tile_t = structured::square::tile::indices<3, 5>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};
    static_assert(t.size() == 125, "");
    static_assert(t.length() == 5, "");

    suint_t c      = 0;
    suint_t x_i    = 0;
    suint_t x_j    = 0;
    suint_t x_k    = 0;
    auto check_all = [&](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == x_i);
      CHECK(i.x[1] == x_j);
      CHECK(i.x[2] == x_k);
      ++c;

      if (c % t.length() == 0) {
        x_i = 0;
        x_j++;
      } else {
        x_i++;
      }

      if (c % (t.length() * t.length()) == 0) {
        x_j = 0;
        x_k++;
      }
      coordinate_tests(i.idx, t.length(), i.x);
    };
    t.for_each(check_all);
    CHECK(c == t.size());
    CHECK(x_i == 0_u);
    CHECK(x_j == 0_u);
    CHECK(x_k == t.length());

    c   = 0;
    x_i = 0;
    x_j = 0;
    x_k = 0;
    RANGES_FOR (auto i, t.sub_tile(x_t(0, 0, 0), x_t(4, 4, 4))) {
      check_all(i);
    }
    CHECK(c == t.size());
    CHECK(x_i == 0_u);
    CHECK(x_j == 0_u);
    CHECK(x_k == t.length());

    c          = 31_u;
    x_i        = 1_u;
    x_j        = 1_u;
    x_k        = 1_u;
    auto check = [&](auto i) {
      // hm3::fmt::print("{}\n", i);
      CHECK(i.idx == i.x.idx());
      CHECK(*i.idx == c);
      CHECK(i.x[0] == x_i);
      CHECK(i.x[1] == x_j);
      CHECK(i.x[2] == x_k);

      if (x_i == 3_u) {
        x_i = 1;
        ++x_j;
        c += 3;
      } else {
        ++x_i;
        ++c;
      }

      if (x_j == 4_u) {
        x_i = 1;
        x_j = 1;
        ++x_k;
        c += 10;
      }
    };
    auto x_min = x_t(1, 1, 1);
    auto x_max = x_t(3, 3, 3);
    t.for_each(x_min, x_max, check);

    c   = 31_u;
    x_i = 1_u;
    x_j = 1_u;
    x_k = 1_u;
    RANGES_FOR (auto i, t.sub_tile(x_min, x_max)) { check(i); }

    {  // rings one:
      auto f = x_t(0, 0, 0);
      auto l = x_t(2, 2, 2);

      suint_t one[] = {
       0, 1, 2,         //
       5, 6, 7,         //
       10, 11, 12,      //
                        //
       25, 26, 27,      //
       30, /*31,*/ 32,  //
       35, 36, 37,      //
       //
       50, 51, 52,  //
       55, 56, 57,  //
       60, 61, 62   //
      };
      int counter_one = 0;
      auto check_one  = [&](auto i) {
        // hm3::fmt::print("{}\n", i);
        CHECK(*i == one[counter_one]);
        ++counter_one;
      };
      tile_t::for_each_ring(f, l, check_one, 0);
      CHECK(counter_one == 26);
    }

    {  // rings two halos: the only internal cell is: 62
      auto f = x_t(0, 0, 0);
      auto l = x_t(4, 4, 4);

      suint_t counter_one = 0;
      auto check_one      = [&](auto i) {
        CHECK(*i == counter_one);

        ++counter_one;
        if (counter_one == 62) { ++counter_one; }
      };
      tile_t::for_each_ring(f, l, check_one, 1);
      CHECK(counter_one == 125);
    }

    closest_cell_tests<tile_t>();
  }

  {  // 6x6x6 tile
    using tile_t = structured::square::tile::indices<3, 6>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};

    {  // test two halo ring:
      auto f = x_t(0, 0, 0);
      auto l = x_t(5, 5, 5);

      int c          = 0;
      auto check_one = [&](auto i) {
        // hm3::fmt::print("{}\n", i);
        CHECK(*i == c);
        ++c;
        if (c == 86 || c == 92 || c == 122 || c == 128) { c += 2; }
      };
      tile_t::for_each_ring(f, l, check_one, 1);
      CHECK(c == 216);
    }
  }
}

void tile_benchs() {
  std::vector<unsigned> v(1000000);
  srand(time(NULL));
  for (auto& e : v) { e = rand(); }

  auto time = [&](auto f) {
    static constexpr auto N        = 10000;
    static constexpr auto N_warmup = 100;
    unsigned long d                = 0;
    int val                        = 0;
    for (int i = 0; i != N_warmup; ++i) { val = f(v); }
    for (int i = 0; i != N; ++i) {
      auto start_ = std::chrono::high_resolution_clock::now();
      val         = f(v);
      auto end_   = std::chrono::high_resolution_clock::now();
      d += (end_ - start_).count();
    }
    return std::make_pair(val, d / N);
  };

  auto fe = [](auto const& v_) {
    using tile_t = structured::square::tile::indices<3, 100>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};
    unsigned val = 0;
    RANGES_FOR (auto&& i, t.sub_tile(x_t(10, 10, 10), x_t(90, 90, 90))) {
      val += v_[*i.idx];
    }
    return val;
  };

  auto fi = [](auto const& v_) {
    using tile_t = structured::square::tile::indices<3, 100>;
    using x_t    = typename tile_t::coordinate;
    constexpr tile_t t{};
    unsigned val = 0;
    t.for_each(x_t(10, 10, 10), x_t(90, 90, 90),
               [&](auto&& i) { val += v_[*i.idx]; });
    return val;
  };

  auto fl = [](auto const& v_) {
    unsigned val = 0;
    for (int k = 10; k <= 90; ++k) {
      for (int j = 10; j <= 90; ++j) {
#pragma clang loop vectorize(enable) interleave(enable)
        for (int i = 10; i <= 90; ++i) {
          int idx = k * 100 * 100 + j * 100 + i;
          val += v_[idx];
        }
      }
    }
    return val;
  };

  const int min_[3] = {10, 10, 10};
  const int max_[3] = {90, 90, 90};

  auto fd = [&min_, &max_](auto const& v_) {
    unsigned val = 0;
    for (int k = min_[2]; k <= max_[2]; ++k) {
      for (int j = min_[1]; j <= max_[1]; ++j) {
        for (int i = min_[0]; i <= max_[0]; ++i) {
          int idx = k * 100 * 100 + j * 100 + i;
          val += v_[idx];
        }
      }
    }
    return val;
  };

  auto fo = [](auto const& v_) {
    unsigned val                = 0;
    constexpr unsigned offset_j = 100 - (90 - 10) - 1;
    constexpr unsigned offset_k = 100 * (100 - (90 - 10) - 1);
    unsigned c                  = 101010;
    for (int k = 10; k <= 90; ++k) {
      for (int j = 10; j <= 90; ++j) {
#pragma clang loop vectorize(enable) interleave(enable)
        for (int i = 10; i <= 90; ++i) {
          val += v_[c];
          ++c;
        }
        c += offset_j;
      }
      c += offset_k;
    }
    return val;
  };

  auto rl = time(fl);
  rl      = time(fl);
  auto rd = time(fd);
  auto ro = time(fo);

  auto re = time(fe);
  auto ri = time(fi);

  std::cout << "int  result: " << ri.first << " time: " << ri.second
            << std::endl;

  std::cout << "ext  result: " << re.first << " time: " << re.second
            << std::endl;

  std::cout << "loop result: " << rl.first << " time: " << rl.second
            << std::endl;

  std::cout << "lopt result: " << ro.first << " time: " << ro.second
            << std::endl;

  std::cout << "ldum result: " << rd.first << " time: " << rd.second
            << std::endl;
}

int main() {
  tile_tests();
#ifdef HM3_ENABLE_BENCHMARKS
  tile_benchs();
#endif
  return test::result();
}
