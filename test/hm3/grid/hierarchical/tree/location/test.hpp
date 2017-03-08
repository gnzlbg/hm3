#pragma once
/// \file
///
/// Location testing functions
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/math/core.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/test.hpp>

template <hm3::uint_t Ad, hm3::uint_t NoLevels, typename Loc>
void test_location(Loc) {
  using namespace hm3;
  using namespace tree;
  static_assert(Movable<Loc>{}, "");
  static_assert(Copyable<Loc>{}, "");
  static_assert(SemiRegular<Loc>{}, "");
  static_assert(EqualityComparable<Loc>{}, "");
  static_assert(Regular<Loc>{}, "");
  static_assert(geometry::AmbientDimension<Loc>{}, "");
  static_assert(TotallyOrdered<Loc>{}, "");
  static_assert(Location<Loc>{}, "");
  Loc l;
  STATIC_CHECK(l.ambient_dimension() == Ad);
  STATIC_CHECK(l.max_no_levels() == NoLevels);
  STATIC_CHECK(*l.max_level() == NoLevels - 1);
  test::check_equal(l.ambient_dimensions(), ambient_dimension[l]);
  {
    auto a        = l;
    uint_t count_ = 0;
    CHECK(a.level() == count_);
    // CHECK(a[0_u] == 1_u);
    for (auto&& c : view::iota(0_su, no_children(Ad))) {
      a.push(c);
      count_++;
      CHECK(a.level() == count_);
      CHECK(a[count_] == c);
    }
    test::check_equal(a.levels(), view::iota(1_u, count_ + 1));
    test::check_equal(a(), view::iota(0_u, no_children(Ad)));
    ranges::for_each(view::iota(0_su, no_children(Ad)) | view::reverse,
                     [&](auto&& c) {
                       CHECK(a.pop() == c);
                       count_--;
                       CHECK(a.level() == count_);
                     });
  }
  {  // test push up to max level (min pip index: 0)
    auto a = l;
    while (a.level() != a.max_level()) { a.push(0_u); }
    auto c = a;
    c.reverse();
    CHECK(c == a);
    CHECK(a.level() == a.max_level());
    {
      using morton_idx = morton_idx_t<Loc>;
      auto a_uint      = static_cast<morton_idx>(a);
      auto a_uint_should
       = math::ipow(morton_idx{2}, morton_idx{Ad * *a.max_level()});

      CHECK(a_uint == a_uint_should);
      CHECK(a_uint == a.morton_idx());
    }

    {
      auto b = a;
      for (auto d : ambient_dimension[l]) {
        hm3::array<int_t, Ad> offset{};
        ranges::fill(offset, int_t{0});

        auto bs = shift(b, offset);
        CHECK(bs);

        offset[d] = -1;

        auto as = shift(b, offset);
        CHECK(!as);
      }
    }
  }
  {  // test push up to max level (max pip index: no_children(Ad) - 1)
    auto a = l;
    while (a.level() != a.max_level()) { a.push(no_children(Ad) - 1_u); }
    CHECK(a.level() == a.max_level());
    morton_idx_t<Loc> r = 0;
    for (auto&& lvl : a.levels()) {
      for (auto&& d : ambient_dimension[l]) {
        bit::set(r, (*lvl - 1) * Ad + d, true);
      }
    }
    bit::set(r, *a.level() * Ad, true);

    CHECK(static_cast<morton_idx_t<Loc>>(a) == r);
    CHECK(a.morton_idx() == r);

    {
      auto b = a;
      for (auto d : ambient_dimension[l]) {
        hm3::array<int_t, Ad> offset{};
        ranges::fill(offset, int_t{0});

        auto bs = shift(b, offset);
        CHECK(bs);

        offset[d] = 1;

        auto as = shift(b, offset);
        CHECK(!as);
      }
    }
  }

  {  // check min
    auto min_loc = Loc::min();
    CHECK(min_loc.level() == min_loc.max_no_levels() - 1);
  }
}

template <template <hm3::dim_t, class...> class Loc>
void test_location_2() {
  using namespace hm3;
  using namespace tree;
  {
    Loc<3> l;
    Loc<3> b;

    CHECK(l.level() == 0_u);
    CHECK(size(l()) == 0_u);
    CHECK(l == b);
    CHECK(!(l != b));

    l = Loc<3>({0, 1, 0, 1});
    CHECK(l.level() == 4_u);
    CHECK(size(l()) == 4_u);
    CHECK(l != b);
    CHECK(!(l == b));

    CHECK(l[1] == 0_u);
    CHECK(l[2] == 1_u);
    CHECK(l[3] == 0_u);
    CHECK(l[4] == 1_u);

    CHECK(l()[0] == 0_u);
    CHECK(l()[1] == 1_u);
    CHECK(l()[2] == 0_u);
    CHECK(l()[3] == 1_u);

    b = l;
    CHECK(b == l);
    b.reverse();
    CHECK(b != l);
    CHECK(b == Loc<3>({1, 0, 1, 0}));
    b.reverse();
    CHECK(b == l);
  }

  {
    auto a           = Loc<2>({3, 2});
    using morton_idx = morton_idx_t<Loc<2>>;
    CHECK(static_cast<morton_idx>(a) == 30_u);
    CHECK(a.morton_idx() == 30_u);
    hm3::array<morton_idx, 2> ar(a);
    CHECK(ar[0] == 2_u);
    CHECK(ar[1] == 3_u);
    CHECK(a.level() == 2_u);
    auto k = shift(a, hm3::array<int_t, 2>{{1, 0}});
    CHECK(!(!k));
    a  = *k;
    ar = static_cast<hm3::array<morton_idx, 2>>(a);
    CHECK(ar[0] == 3_u);
    CHECK(ar[1] == 3_u);
    CHECK(ar[0] == a.morton_x()[0]);
    CHECK(ar[1] == a.morton_x()[1]);

    CHECK(a == Loc<2>({3, 3}));
  }
  {  // test to_int coordinate-wise
    auto a           = Loc<2>({2, 1});
    using morton_idx = morton_idx_t<Loc<2>>;
    hm3::array<morton_idx, 2> ar(a);
    CHECK(ar[0] == 1_u);
    CHECK(ar[1] == 2_u);
    CHECK(a.morton_x()[0] == 1_u);
    CHECK(a.morton_x()[1] == 2_u);

    a  = *shift(a, hm3::array<int_t, 2>{{-1, 0}});
    ar = static_cast<hm3::array<morton_idx, 2>>(a);
    CHECK(ar[0] == 0_u);
    CHECK(ar[1] == 2_u);
    CHECK(a.morton_x()[0] == 0_u);
    CHECK(a.morton_x()[1] == 2_u);

    CHECK(a == Loc<2>({2, 0}));
  }
  {  // test to_int coordinate-wise
    auto a           = Loc<2>({3, 0, 0});
    using morton_idx = morton_idx_t<Loc<2>>;
    hm3::array<morton_idx, 2> ar(a);
    CHECK(ar[0] == 4_u);
    CHECK(ar[1] == 4_u);
    CHECK(a.morton_x()[0] == 4_u);
    CHECK(a.morton_x()[1] == 4_u);

    a  = *shift(a, hm3::array<int_t, 2>{{-1, -1}});
    ar = static_cast<hm3::array<morton_idx, 2>>(a);
    CHECK(ar[0] == 3_u);
    CHECK(ar[1] == 3_u);
    CHECK(a.morton_x()[0] == 3_u);
    CHECK(a.morton_x()[1] == 3_u);

    CHECK(a == Loc<2>({0, 3, 3}));
  }
  {  // test push up to max level
    Loc<2> l;
    for (uint_t i = 0, e = *l.max_level(); i < e; ++i) { l.push(0_u); }
    CHECK(l.level() == l.max_level());
  }

  {  // test to_int coordinate-wise
    auto a           = Loc<2>({1, 1, 2});
    using morton_idx = morton_idx_t<Loc<2>>;
    hm3::array<morton_idx, 2> ar(a);
    CHECK(ar[0] == 6_u);
    CHECK(ar[1] == 1_u);
    CHECK(a.morton_x()[0] == 6_u);
    CHECK(a.morton_x()[1] == 1_u);
  }

  {  // test to_int coordinate-wise
    auto a           = Loc<3>({6, 5});
    using morton_idx = morton_idx_t<Loc<3>>;
    hm3::array<morton_idx, 3> ar(a);
    CHECK(a.level() == 2_u);
    CHECK(ar[0] == 1_u);
    CHECK(ar[1] == 2_u);
    CHECK(ar[2] == 3_u);
    CHECK(a.morton_x()[0] == 1_u);
    CHECK(a.morton_x()[1] == 2_u);
    CHECK(a.morton_x()[2] == 3_u);
  }

  // test float from Loc
  {
    {  // loc (0.25, 0.75)
      hm3::array<num_t, 2> x{{0.25, 0.75}};
      {
        auto fl = Loc<2>(x, 1);
        CHECK(fl.level() == 1_u);
        CHECK(size(fl()) == 1_u);
        CHECK(fl[1] == 2_u);
        CHECK(fl()[0] == 2_u);
      }
      {
        auto fl = Loc<2>(x, 0);
        CHECK(fl.level() == 0_u);
        CHECK(size(fl()) == 0_u);
      }
    }
    {  // loc (0.6, 0.8)
      hm3::array<num_t, 2> x{{0.6, 0.8}};
      {
        auto fl = Loc<2>(x, 1);
        CHECK(fl.level() == 1_u);
        CHECK(size(fl()) == 1_u);
        CHECK(fl[1] == 3_u);
        CHECK(fl()[0] == 3_u);
      }
      {
        auto fl = Loc<2>(x, 2);
        CHECK(fl.level() == 2_u);
        CHECK(size(fl()) == 2_u);
        CHECK(fl[1] == 3_u);
        CHECK(fl()[0] == 3_u);
        CHECK(fl[2] == 2_u);
        CHECK(fl()[1] == 2_u);
      }
      {
        auto fl = Loc<2>(x, 3);
        CHECK(fl.level() == 3_u);
        CHECK(size(fl()) == 3_u);
        CHECK(fl[1] == 3_u);
        CHECK(fl()[0] == 3_u);
        CHECK(fl[2] == 2_u);
        CHECK(fl()[1] == 2_u);
        CHECK(fl[3] == 0_u);
        CHECK(fl()[2] == 0_u);
      }
    }
  }
}
