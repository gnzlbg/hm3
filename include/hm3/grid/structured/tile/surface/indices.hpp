#pragma once
/// \file
///
/// Structured square tile's surface indices
///
/// TODO: remove the cell_bounds::length variants (use the general one)
/// TODO: add an iteration that maximizes data locality
#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/grid/structured/tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/cell/indices.hpp>
#include <hm3/grid/structured/tile/surface/bounds.hpp>
#include <hm3/grid/structured/tile/surface/coordinate.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {

namespace surface {

/// Square structured tile surface indices
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per dimension
template <dim_t Nd, tidx_t Nc>
struct indices : bounds<Nd, Nc> {
  using self            = indices<Nd, Nc>;
  using cell_bounds     = cell::bounds<Nd, Nc>;
  using cell_coordinate = cell::coordinate<Nd, Nc>;
  using cell_indices    = cell::indices<Nd, Nc>;
  using coordinate      = coordinate<Nd, Nc>;

  /// For each surface of cell \p c
  template <typename F>
  static constexpr void for_each(cell_coordinate c, F&& f) noexcept {
    for (dim_t d = 0; d < Nd; ++d) {
      f(coordinate(c, d));
      auto neighbor_pos = c.offset(d, +1);
      f(coordinate(neighbor_pos, d));
    }
  }

  /// For each surface in the tile with normal component in direction \p d
  template <typename F, CONCEPT_REQUIRES_(Nd == 1)>
  static constexpr void for_each(F&& f, suint_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);
    for (tidx_t i = 0; i < self::size(); ++i) {
      f(coordinate(cell_coordinate(i), 0));
    }
  }

  /// For each surface in the tile with normal component in direction \p d
  template <typename F, CONCEPT_REQUIRES_(Nd == 2)>
  static constexpr void for_each(F&& f, dim_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);
    switch (d) {
      case 0: {
        for (tidx_t j = 0; j < cell_bounds::length(); ++j) {
          for (tidx_t i = 0; i < cell_bounds::length() + 1; ++i) {
            f(coordinate(cell_coordinate(i, j), 0));
          }
        }
        return;
      }
      case 1: {
        for (tidx_t i = 0; i < cell_bounds::length(); ++i) {
          for (tidx_t j = 0; j < cell_bounds::length() + 1; ++j) {
            f(coordinate(cell_coordinate(i, j), 1));
          }
        }
        return;
      }
      default: { HM3_UNREACHABLE(); }
    }
  }

  /// For each surface in the tile with normal component in direction \p d
  template <typename F, CONCEPT_REQUIRES_(Nd == 3)>
  static constexpr void for_each(F&& f, dim_t d) noexcept {
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);
    switch (d) {
      case 0: {
        for (tidx_t k = 0; k < cell_bounds::length(); ++k) {
          for (tidx_t j = 0; j < cell_bounds::length(); ++j) {
            for (tidx_t i = 0; i < cell_bounds::length() + 1; ++i) {
              f(coordinate(cell_coordinate(i, j, k), 0));
            }
          }
        }
        return;
      }
      case 1: {
        for (tidx_t k = 0; k < cell_bounds::length(); ++k) {
          for (tidx_t i = 0; i < cell_bounds::length(); ++i) {
            for (tidx_t j = 0; j < cell_bounds::length() + 1; ++j) {
              f(coordinate(cell_coordinate(i, j, k), 1));
            }
          }
        }
        return;
      }
      case 2: {
        for (tidx_t j = 0; j < cell_bounds::length(); ++j) {
          for (tidx_t i = 0; i < cell_bounds::length(); ++i) {
            for (tidx_t k = 0; k < cell_bounds::length() + 1; ++k) {
              f(coordinate(cell_coordinate(i, j, k), 2));
            }
          }
        }
        return;
      }
      default: { HM3_UNREACHABLE(); }
    }
  }

  /// For each surface in the tile
  template <typename F>
  [[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]]  //
   static constexpr void
   for_each(F&& f) noexcept {
    for (dim_t d = 0; d < Nd; ++d) { for_each(f, d); }
  }

  /// For each surface in the tile with normal component in direction \p d
  template <typename F, CONCEPT_REQUIRES_(Nd == 1)>
  static constexpr void for_each(const cell_coordinate from,
                                 const cell_coordinate to, F&& f,
                                 dim_t d) noexcept {
    cell_indices::assert_from_to(from, to);
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);
    for (tidx_t i = from[0]; i <= to[0] + 1; ++i) {
      f(coordinate(cell_coordinate(i), 0));
    }
  }

  /// For each surface in the tile with normal component in direction \p d
  template <typename F, CONCEPT_REQUIRES_(Nd == 2)>
  static constexpr void for_each(const cell_coordinate from,
                                 const cell_coordinate to, F&& f,
                                 dim_t d) noexcept {
    cell_indices::assert_from_to(from, to);
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);
    switch (d) {
      case 0: {
        for (tidx_t j = from[1]; j <= to[1]; ++j) {
          for (tidx_t i = from[0]; i <= to[0] + 1; ++i) {
            f(coordinate(cell_coordinate(i, j), 0));
          }
        }
        return;
      }
      case 1: {
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          for (tidx_t j = from[1]; j <= to[0] + 1; ++j) {
            f(coordinate(cell_coordinate(i, j), 1));
          }
        }
        return;
      }
      default: { HM3_UNREACHABLE(); }
    }
  }

  template <typename F, CONCEPT_REQUIRES_(Nd == 3)>
  static constexpr void for_each(const cell_coordinate from,
                                 const cell_coordinate to, F&& f,
                                 dim_t d) noexcept {
    cell_indices::assert_from_to(from, to);
    HM3_ASSERT(d < Nd,
               "direction {} out-of-bounds for {}-dimensional tile: [0, {})", d,
               Nd, Nd);

    switch (d) {
      case 0: {
        for (tidx_t k = from[2]; k <= to[2]; ++k) {
          for (tidx_t j = from[1]; j <= to[1]; ++j) {
            for (tidx_t i = from[0]; i <= to[0] + 1; ++i) {
              f(coordinate(cell_coordinate(i, j, k), 0));
            }
          }
        }
        return;
      }
      case 1: {
        for (tidx_t k = from[2]; k <= to[2]; ++k) {
          for (tidx_t i = from[0]; i <= to[0]; ++i) {
            for (tidx_t j = from[1]; j <= to[1] + 1; ++j) {
              f(coordinate(cell_coordinate(i, j, k), 1));
            }
          }
        }
        return;
      }
      case 2: {
        for (tidx_t j = from[1]; j <= to[1]; ++j) {
          for (tidx_t i = from[0]; i <= to[0]; ++i) {
            for (tidx_t k = from[2]; k <= to[2] + 1; ++k) {
              f(coordinate(cell_coordinate(i, j, k), 2));
            }
          }
        }
        return;
      }
      default: { HM3_UNREACHABLE(); }
    }
  }

  template <typename F>
  [[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]] static constexpr void for_each(
   cell_coordinate from, cell_coordinate to, F&& f) noexcept {
    cell_indices::assert_from_to(from, to);
    for (dim_t d = 0; d < Nd; ++d) { for_each(from, to, f, d); }
  }
};

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
