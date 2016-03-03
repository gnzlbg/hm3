#pragma once
/// \file
///
/// Square structured grid halo-cell relations
#include <array>
#include <hm3/grid/structured/bounds.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {
namespace grid {
namespace structured {

template <suint_t Nd> struct halo_tile { std::array<suint_t, Nd> from, to; };

template <suint_t Nd, suint_t M, suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table {
  static constexpr std::array<halo_tile<Nd>, 0> indices{};
};

// 1D face halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<1, 0, Nic, Nhl> {
  using b = bounds<1, Nic, Nhl>;
  static constexpr std::array<halo_tile<1>, 2> indices{{
   //
   {              // 0:
    {{b::fhl}},   // from
    {{b::lhl}}},  // to
   {              // 1:
    {{b::fhr}},   // from
    {{b::lhr}}},  // to
                  //
  }};
};

// 2D face halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<2, 0, Nic, Nhl> {
  using b = bounds<2, Nic, Nhl>;
  static constexpr std::array<halo_tile<2>, 4> indices{{
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
    {{b::li, b::lhr}}},  // to
                         //

  }};
};

// 2D edge (corner) halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<2, 1, Nic, Nhl> {
  using b = bounds<2, Nic, Nhl>;
  static constexpr std::array<halo_tile<2>, 4> indices{{
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
    {{b::lhr, b::lhr}}},  // to
                          //
  }};
};

// 3D face halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<3, 0, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr std::array<halo_tile<3>, 6> indices{{
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
    {{b::li, b::li, b::lhr}}},  // to
                                //

  }};
};

// 3D edge halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<3, 1, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr std::array<halo_tile<3>, 12> indices{{
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
    {{b::li, b::lhr, b::lhr}}},  // to

   //
  }};
};

// 3D corner halo tiles
template <suint_t Nic, suint_t Nhl>  //
struct halo_tiles_lookup_table<3, 2, Nic, Nhl> {
  using b = bounds<3, Nic, Nhl>;
  static constexpr std::array<halo_tile<3>, 8> indices{{
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
    {{b::lhr, b::lhr, b::lhr}}},  // to

   //
  }};
};

template <typename Neighbor, typename Bounds>
constexpr auto halo_tile_bounds(Neighbor pos, Bounds) noexcept {
  using manifold      = get_tag_t<Neighbor>;
  constexpr auto rank = manifold::rank();
  constexpr auto nd   = manifold::dimension();
  constexpr auto nic  = Bounds::internal_cells();
  constexpr auto nhl  = Bounds::halo_layers();
  return halo_tiles_lookup_table<nd, rank, nic, nhl>::indices[*pos];
}

}  // namespace structured
}  // namespace grid
}  // namespace hm3
