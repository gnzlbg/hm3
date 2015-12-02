#pragma once
/// \file
#include <hm3/geometry/dimensions.hpp>

namespace hm3 {
namespace solver {
namespace lbm {
namespace lattice {

/// D2Q9 Lattice:
///
///  7   3   4     NW  N  NE
///
///  0   8   1      W  C  E
///
///  6   2   5     SW  S  SE
///
struct d2q9 : dimensional<2> {
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
    W  = 3,
    E  = 1,
    S  = 4,
    N  = 2,
    NE = 5,
    SE = 8,
    SW = 7,
    NW = 6,
    C  = 0
  };
  static auto without_center() noexcept { return view::iota(1, 9); }
  static auto direct() noexcept { return view::iota(1, 5); }
  static auto diagonal() noexcept { return view::iota(5, 9); }

  static constexpr suint_t opposite(suint_t d) noexcept {
    suint_t stencil[size()] = {0};

    stencil[W]  = E;
    stencil[E]  = W;
    stencil[S]  = N;
    stencil[N]  = S;
    stencil[NE] = SW;
    stencil[SE] = NW;
    stencil[SW] = NE;
    stencil[NW] = SE;
    stencil[C]  = C;
    return stencil[d];
  }

  static constexpr auto dir(suint_t d) noexcept {
    using dir_t   = std::array<sint_t, 2>;
    dir_t dirs[9] = {{{0, 0}}};

    dirs[W]  = dir_t{{-1, 0}};
    dirs[E]  = dir_t{{1, 0}};
    dirs[S]  = dir_t{{0, -1}};
    dirs[N]  = dir_t{{0, 1}};
    dirs[NE] = dir_t{{1, 1}};
    dirs[SE] = dir_t{{1, -1}};
    dirs[SW] = dir_t{{-1, -1}};
    dirs[NW] = dir_t{{-1, 1}};
    dirs[C]  = dir_t{{0, 0}};
    return dirs[d];
  }

  static constexpr auto periodic_neighbor_dir_x(suint_t d) noexcept {
    using dir_t   = std::array<sint_t, 2>;
    dir_t dirs[9] = {{{0, 0}}};

    dirs[W]  = dir_t{{-1, 0}};
    dirs[E]  = dir_t{{1, 0}};
    dirs[S]  = dir_t{{0, 0}};
    dirs[N]  = dir_t{{0, 0}};
    dirs[NE] = dir_t{{1, 1}};
    dirs[SE] = dir_t{{1, 1}};
    dirs[SW] = dir_t{{-1, -1}};
    dirs[NW] = dir_t{{-1, -1}};
    dirs[C]  = dir_t{{0, 0}};
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

    t[W]  = t_direct;
    t[E]  = t_direct;
    t[S]  = t_direct;
    t[N]  = t_direct;
    t[NE] = t_diagonal;
    t[SE] = t_diagonal;
    t[SW] = t_diagonal;
    t[NW] = t_diagonal;
    t[C]  = t_center;
    return t[d];
  }
};

}  // namespace lattice
}  // namespace lbm
}  // namespace solver
}  // namespace hm3
