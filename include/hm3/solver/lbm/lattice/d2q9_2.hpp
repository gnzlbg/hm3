#pragma once
/// \file

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
struct d2q9_2 : dimensional<2> {
  // the members of the enum are exported into scope
  enum distributions_e {
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
  static constexpr suint_t size() noexcept { return 9; }
  static auto all() noexcept { return view::iota(0, no_distributions()); }
  static auto without_center() noexcept { return view::iota(0, 8); }
  static auto direct() noexcept { return view::iota(0, 4); }
  static auto diagonal() noexcept { return view::iota(4, 8); }

  static constexpr suint_t os[9] = {1, 0, 3, 2, 6, 7, 4, 5, 0};
  static constexpr suint_t opposite(suint_t d) noexcept { return os[d]; }

  static constexpr auto dir(suint_t d) noexcept {
    std::array<std::array<sint_t, 2>, 9> dirs;
    dirs
    suint_t d_ts[] = {
     0, 1, 2, 3,  // direct neighbors
     3, 1, 0, 2   // edge neighbors
    };
    suint_t d_t = d_ts[d];
    return (d < 4) ? tree::face_neighbors<2>()[d_t]
                   : tree::edge_neighbors<2>()[d_t];
  }

  static constexpr num_t cs() noexcept { return 1. / std::sqrt(3.); }

  static constexpr num_t constants(suint_t d) noexcept {
    HM3_ASSERT(d >= 0 and d < size(), "distribution is out of bounds");
    const num_t t_center   = 4. / 9.;
    const num_t t_direct   = 1. / 9.;
    const num_t t_diagonal = 1. / 36.;
    num_t t[9];
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
