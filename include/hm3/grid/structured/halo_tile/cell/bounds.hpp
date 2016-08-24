#pragma once
/// \file
///
/// Square structured halo tile bounds
#include <hm3/geometry/dimension.hpp>
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/grid/structured/halo_tile/index_type.hpp>
#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {
namespace cell {

namespace detail {

constexpr tidx_t cells_per_length(tidx_t nic, tidx_t nhl) noexcept {
  return 2 * nhl + nic;
}
constexpr tidx_t first_halo_left(tidx_t /*nic*/, tidx_t /* nhl*/) noexcept {
  return 0;
}
constexpr tidx_t last_halo_left(tidx_t /*nic*/, tidx_t nhl) noexcept {
  return nhl - 1;
}
constexpr tidx_t first_internal(tidx_t /*nic*/, tidx_t nhl) noexcept {
  return nhl;
}
constexpr tidx_t last_internal(tidx_t nic, tidx_t nhl) noexcept {
  return nhl + nic - 1;
}
constexpr tidx_t first_halo_right(tidx_t nic, tidx_t nhl) noexcept {
  return nhl + nic;
}
constexpr tidx_t last_halo_right(tidx_t nic, tidx_t nhl) noexcept {
  return 2 * nhl + nic - 1;
}

template <dim_t Nd>
struct subtile_bounds {
  array<tile::tidx_t, Nd> from, to;
};

}  // namespace detail

/// Bounds of a square structured grid
///
/// Nd: number of spatial dimensions
/// Nic: number of internal cells per dimension
/// Nhl: number of of halo layers
///
template <dim_t Nd, tidx_t Nic, tidx_t Nhl>
struct bounds : tile::cell::bounds<Nd, detail::cells_per_length(Nic, Nhl)> {
  using self = bounds;
  /// \name Sizes
  ///@{

  /// Number of halo layers
  static constexpr tidx_t halo_layers() noexcept { return Nhl; }
  /// Number of internal cells per length
  static constexpr tidx_t internal_cell_length() noexcept { return Nic; }
  /// Total number of internal cells
  static constexpr tidx_t internal_cell_size() noexcept {
    return math::ipow(internal_cell_length(), static_cast<tidx_t>(Nd));
  }
  /// Total number of halo cells
  static constexpr tidx_t halo_cell_size() noexcept {
    return self::size() - internal_cell_size();
  }

  ///@} // Sizes

  /// \name Coordinates
  ///@{

  /// 1D Coordinate of first halo cell of the tile with coordinates smaller than
  /// the internal cells
  static constexpr tidx_t first_halo_left() noexcept {
    return detail::first_halo_left(Nic, Nhl);
  }
  /// 1D Coordinate of last halo cell of the tile with coordinates smaller than
  /// the internal cells
  static constexpr tidx_t last_halo_left() noexcept {
    return detail::last_halo_left(Nic, Nhl);
  }
  /// 1D Coordinate of first internal cell
  static constexpr tidx_t first_internal() noexcept {
    return detail::first_internal(Nic, Nhl);
  }
  /// 1D Coordinate of last internal cell
  static constexpr tidx_t last_internal() noexcept {
    return detail::last_internal(Nic, Nhl);
  }
  /// 1D Coordinate of first internal cell (including \p h halo layers)
  static constexpr tidx_t first_internal(tidx_t h) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    return detail::first_internal(Nic, Nhl) - h;
  }
  /// 1D Coordinate of last internal cell (including \p h halo layers)
  static constexpr tidx_t last_internal(tidx_t h) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    return detail::last_internal(Nic, Nhl) + h;
  }
  /// 1D Coordinate of first halo cell of the tile with coordinates larger than
  /// the internal cells
  static constexpr tidx_t first_halo_right() noexcept {
    return detail::first_halo_right(Nic, Nhl);
  }
  /// 1D Coordinate of last halo cell of the tile with coordinates larger than
  /// the internal cells
  static constexpr tidx_t last_halo_right() noexcept {
    return detail::last_halo_right(Nic, Nhl);
  }
  // static constexpr auto cpl = self::length();
  static constexpr auto fhl = first_halo_left();
  static constexpr auto lhl = last_halo_left();
  static constexpr auto fi  = first_internal();
  static constexpr auto li  = last_internal();
  static constexpr auto fhr = first_halo_right();
  static constexpr auto lhr = last_halo_right();

  ///@}  // Coordinates

  static constexpr detail::subtile_bounds<Nd> internal_cells() noexcept {
    detail::subtile_bounds<Nd> b;
    for (dim_t d = 0; d < Nd; ++d) {
      b.from[d] = fi;
      b.to[d]   = li;
    }
    return b;
  }
};

namespace detail {

template <dim_t Nd, tidx_t M, tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_ {
  static constexpr array<subtile_bounds<0>, 0> indices{};
};

// 1D face halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<1, 1, Nic, Nhl> {
  using b = bounds<1, Nic, Nhl>;
  static constexpr array<subtile_bounds<1>, 2> indices{{
   //
   {              // 0:
    {{b::fhl}},   // from
    {{b::lhl}}},  // to
   {              // 1:
    {{b::fhr}},   // from
    {{b::lhr}}}   // to
  }};
};

// 2D face halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<2, 1, Nic, Nhl> {
  using b = bounds<2, Nic, Nhl>;
  static constexpr array<subtile_bounds<2>, 4> indices{{
   //
   {                     // 0:
    {{b::fhl, b::fi}},   // from
    {{b::lhl, b::li}}},  // to
   {                     // 1:
    {{b::fhr, b::fi}},   // from
    {{b::lhr, b::li}}},  // to
   {                     // 2:
    {{b::fi, b::fhl}},   // from
    {{b::li, b::lhl}}},  // to
   {                     // 3:
    {{b::fi, b::fhr}},   // from
    {{b::li, b::lhr}}}   // to
  }};
};

// 2D edge (corner) halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<2, 2, Nic, Nhl> {
  using b = bounds<2, Nic, Nhl>;
  static constexpr array<subtile_bounds<2>, 4> indices{{
   //
   {                      // 0:
    {{b::fhl, b::fhl}},   // from
    {{b::lhl, b::lhl}}},  // to
   {                      // 1:
    {{b::fhr, b::fhl}},   // from
    {{b::lhr, b::lhl}}},  // to
   {                      // 2:
    {{b::fhl, b::fhr}},   // from
    {{b::lhl, b::lhr}}},  // to
   {                      // 3:
    {{b::fhr, b::fhr}},   // from
    {{b::lhr, b::lhr}}}   // to
  }};
};

// 3D face halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<3, 1, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr array<subtile_bounds<3>, 6> indices{{
   //
   {                            // 0:
    {{b::fhl, b::fi, b::fi}},   // from
    {{b::lhl, b::li, b::li}}},  // to
   {                            // 1:
    {{b::fhr, b::fi, b::fi}},   // from
    {{b::lhr, b::li, b::li}}},  // to
   {                            // 2:
    {{b::fi, b::fhl, b::fi}},   // from
    {{b::li, b::lhl, b::li}}},  // to
   {                            // 3:
    {{b::fi, b::fhr, b::fi}},   // from
    {{b::li, b::lhr, b::li}}},  // to
   {                            // 4:
    {{b::fi, b::fi, b::fhl}},   // from
    {{b::li, b::li, b::lhl}}},  // to
   {                            // 5:
    {{b::fi, b::fi, b::fhr}},   // from
    {{b::li, b::li, b::lhr}}}   // to
  }};
};

// 3D edge halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<3, 2, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr array<subtile_bounds<3>, 12> indices{{
   //
   {                             // 0:
    {{b::fhl, b::fhl, b::fi}},   // from
    {{b::lhl, b::lhl, b::li}}},  // to
   {                             // 1:
    {{b::fhr, b::fhl, b::fi}},   // from
    {{b::lhr, b::lhl, b::li}}},  // to
   {                             // 2:
    {{b::fhl, b::fhr, b::fi}},   // from
    {{b::lhl, b::lhr, b::li}}},  // to
   {                             // 3:
    {{b::fhr, b::fhr, b::fi}},   // from
    {{b::lhr, b::lhr, b::li}}},  // to
   {                             // 4:
    {{b::fhl, b::fi, b::fhl}},   // from
    {{b::lhl, b::li, b::lhl}}},  // to
   {                             // 5:
    {{b::fhr, b::fi, b::fhl}},   // from
    {{b::lhr, b::li, b::lhl}}},  // to
   {                             // 6:
    {{b::fi, b::fhl, b::fhl}},   // from
    {{b::li, b::lhl, b::lhl}}},  // to
   {                             // 7:
    {{b::fi, b::fhr, b::fhl}},   // from
    {{b::li, b::lhr, b::lhl}}},  // to
   {                             // 8:
    {{b::fhl, b::fi, b::fhr}},   // from
    {{b::lhl, b::li, b::lhr}}},  // to
   {                             // 9:
    {{b::fhr, b::fi, b::fhr}},   // from
    {{b::lhr, b::li, b::lhr}}},  // to
   {                             // 10:
    {{b::fi, b::fhl, b::fhr}},   // from
    {{b::li, b::lhl, b::lhr}}},  // to
   {                             // 11:
    {{b::fi, b::fhr, b::fhr}},   // from
    {{b::li, b::lhr, b::lhr}}}   // to
  }};
};

// 3D corner halo tiles
template <tidx_t Nic, tidx_t Nhl>
struct halo_tiles_lookup_table_<3, 3, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr array<subtile_bounds<3>, 8> indices{{
   //
   {                              // 0:
    {{b::fhl, b::fhl, b::fhl}},   // from
    {{b::lhl, b::lhl, b::lhl}}},  // to
   {                              // 1:
    {{b::fhr, b::fhl, b::fhl}},   // from
    {{b::lhr, b::lhl, b::lhl}}},  // to
   {                              // 2:
    {{b::fhl, b::fhr, b::fhl}},   // from
    {{b::lhl, b::lhr, b::lhl}}},  // to
   {                              // 3:
    {{b::fhr, b::fhr, b::fhl}},   // from
    {{b::lhr, b::lhr, b::lhl}}},  // to
   {                              // 4:
    {{b::fhl, b::fhl, b::fhr}},   // from
    {{b::lhl, b::lhl, b::lhr}}},  // to
   {                              // 5:
    {{b::fhr, b::fhl, b::fhr}},   // from
    {{b::lhr, b::lhl, b::lhr}}},  // to
   {                              // 6:
    {{b::fhl, b::fhr, b::fhr}},   // from
    {{b::lhl, b::lhr, b::lhr}}},  // to
   {                              // 7:
    {{b::fhr, b::fhr, b::fhr}},   // from
    {{b::lhr, b::lhr, b::lhr}}}   // to
  }};
};

namespace {
template <dim_t Nd, tidx_t M, tidx_t Nic, tidx_t Nhl>
static constexpr auto halo_tiles_lookup_table
 = halo_tiles_lookup_table_<Nd, M, Nic, Nhl>::indices;
}  // namespace

}  // namespace detail

template <typename Neighbor, typename Bounds>
constexpr auto halos(Neighbor pos, Bounds) noexcept {
  using manifold     = get_tag_t<Neighbor>;
  constexpr auto r   = manifold::rank();
  constexpr auto nd  = manifold::dimension();
  constexpr auto nic = Bounds::internal_cell_length();
  constexpr auto nhl = Bounds::halo_layers();
  return detail::halo_tiles_lookup_table<nd, r, nic, nhl>[*pos];
}

}  // namespace cell
}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
