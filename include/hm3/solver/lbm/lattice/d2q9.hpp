#pragma once
/// \file
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/utility/array.hpp>

namespace hm3::solver::lbm::lattice {

/// D2Q9 Lattice:
///
///  7   3   4     NW  N  NE
///
///  0   8   1      W  C  E
///
///  6   2   5     SW  S  SE
///
struct d2q9 : ::hm3::geometry::with_ambient_dimension<2> {
  static constexpr suint_t size() noexcept { return 9; }
  static auto all() noexcept { return view::iota(suint_t{0}, size()); }
  /*
  // the members of the enum are exported into scope
  enum indices {
    W  = 0,
    E  = 1,
    S  = 2,
    N  = 3,
    NE = 4,
    SE = 5,
    SW = 6,
    NW = 7,
    C  = 8
  };
  static auto without_center() noexcept { return view::iota(0, 8); }
  static auto direct() noexcept { return view::iota(0, 4); }
  static auto diagonal() noexcept { return view::iota(4, 8); }
  */

  enum indices {
    w  = 3,
    e  = 1,
    s  = 4,
    n  = 2,
    ne = 5,
    se = 8,
    sw = 7,
    nw = 6,
    c  = 0
  };
  static auto without_center() noexcept { return view::iota(1, 9); }
  static auto direct() noexcept { return view::iota(1, 5); }
  static auto diagonal() noexcept { return view::iota(5, 9); }

  static constexpr suint_t opposite(suint_t d) noexcept {
    suint_t stencil[size()] = {0};
    stencil[w]              = e;
    stencil[e]              = w;
    stencil[s]              = n;
    stencil[n]              = s;
    stencil[ne]             = sw;
    stencil[se]             = nw;
    stencil[sw]             = ne;
    stencil[nw]             = se;
    stencil[c]              = c;
    return stencil[d];
  }

  static constexpr auto dir(suint_t d) noexcept {
    using dir_t   = array<sint_t, 2>;
    dir_t dirs[9] = {{{0, 0}}};

    dirs[w]  = dir_t{{-1, 0}};
    dirs[e]  = dir_t{{1, 0}};
    dirs[s]  = dir_t{{0, -1}};
    dirs[n]  = dir_t{{0, 1}};
    dirs[ne] = dir_t{{1, 1}};
    dirs[se] = dir_t{{1, -1}};
    dirs[sw] = dir_t{{-1, -1}};
    dirs[nw] = dir_t{{-1, 1}};
    dirs[c]  = dir_t{{0, 0}};
    return dirs[d];
  }

  static constexpr auto periodic_neighbor_dir_x(suint_t d) noexcept {
    using dir_t   = array<sint_t, 2>;
    dir_t dirs[9] = {{{0, 0}}};

    dirs[w]  = dir_t{{-1, 0}};
    dirs[e]  = dir_t{{1, 0}};
    dirs[s]  = dir_t{{0, 0}};
    dirs[n]  = dir_t{{0, 0}};
    dirs[ne] = dir_t{{1, 1}};
    dirs[se] = dir_t{{1, 1}};
    dirs[sw] = dir_t{{-1, -1}};
    dirs[nw] = dir_t{{-1, -1}};
    dirs[c]  = dir_t{{0, 0}};
    return dirs[d];
  }

  static num_t cs() noexcept { return 1. / std::sqrt(3.); }
  static num_t cs_2() noexcept { return cs() * cs(); }

  static constexpr num_t constants(suint_t d) noexcept {
    HM3_ASSERT(d >= 0 and d < size(), "distribution is out of bounds");
    const num_t t_center   = 4. / 9.;
    const num_t t_direct   = 1. / 9.;
    const num_t t_diagonal = 1. / 36.;

    num_t t[9] = {0};

    t[w]  = t_direct;
    t[e]  = t_direct;
    t[s]  = t_direct;
    t[n]  = t_direct;
    t[ne] = t_diagonal;
    t[se] = t_diagonal;
    t[sw] = t_diagonal;
    t[nw] = t_diagonal;
    t[c]  = t_center;
    return t[d];
  }
};

}  // namespace hm3::solver::lbm::lattice
