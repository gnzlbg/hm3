#pragma once
/// \file
///
/// Square structured grid
//#include <hm3/grid/structured/halo_tile.hpp>
//#include <hm3/grid/structured/tile.hpp>

namespace hm3 {
namespace grid {

/// Square structured grid algorithms
namespace structured {

/// Square structured grid
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of internal cells per length
/// \tparam Nhl number of halo layers
///
/// \note A square structured grid is by default a square structured tile with
/// halo layers in case they are needed.
///
// template <suint_t Nd, suint_t Nic, suint_t Nhl = 0>
// using grid
//  = std::conditional_t<(Nhl > 0),  // if it has halo layers, then halo tile:
//                       halo_tile<Nd, Nic, Nhl>,
//                       // otherwise, just a tile:
//                       tile<Nd, Nic>>;

// /// Square structured grid
// ///
// /// \tparam Nd number of spatial dimensions
// /// \tparam Nic number of internal cells per length
// /// \tparam Nhl number of halo layers
// template <suint_t Nd, suint_t Nic, suint_t Nhl>
// struct grid : geometry::dimensional<Nd> {
//   using cell_indices = cell_indices<Nd, Nic, Nhl>;
//   using cell_t       = cell<Nd, Nic, Nhl>;
//   using square_t     = geometry::square<Nd>;
//   using point_t      = geometry::point<Nd>;
//   using coordinate_t = typename cell_indices::coordinate_t;

//   square_t bounding_box_;
//   point_t x_first_cell_;
//   num_t cell_length_;

//   /// Grid length
//   constexpr num_t length() const noexcept {
//     return geometry::length(bounding_box_);
//   }
//   /// Cell length
//   constexpr num_t cell_length() const noexcept { return cell_length_; }

//   /// Cell volume
//   constexpr num_t cell_volume() const noexcept {
//     return std::pow(cell_length(), Nd);
//   }

//   /// Cell surface area
//   constexpr num_t cell_surface_area() const noexcept {
//     return std::pow(cell_length(), Nd - 1);
//   }

//   /// Sets the cell length of the grid to \p new_cell_length
//   constexpr void set_cell_length(num_t new_cell_length) noexcept {
//     HM3_ASSERT(new_cell_length > 0., "cell length {} is not > 0.",
//                new_cell_length);
//     cell_length_ = new_cell_length;
//   }

//   /// Compute the cell length from the length of the block
//   static constexpr num_t compute_cell_length(num_t block_length) noexcept {
//     HM3_ASSERT(block_length > 0., "block length {} must be positive",
//                block_length);
//     return block_length / cell_indices::internal_cells_per_length();
//   }

//   /// Computes first cell coordinates (idx = 0) from bounding box coordinates
//   static constexpr point_t compute_first_cell_coordinates(
//    square_t bbox) noexcept {
//     HM3_ASSERT(geometry::length(bbox) > 0.,
//                "bounding box length must be positive, bbox {}", bbox);
//     auto x_min  = geometry::bounds(bbox).min;
//     auto cell_l = compute_cell_length(geometry::length(bbox));
//     for (suint_t d = 0; d < Nd; ++d) {
//       x_min(d) += cell_l / 2. - cell_l * cell_indices::halo_layers();
//     }
//     return x_min;
//   }

//   /// Block's bounding box
//   constexpr square_t bounding_box() const noexcept { return bounding_box_; }

//   /// Cell at coordinate \p x
//   constexpr cell_t cell(coordinate_t x) const noexcept {
//     HM3_ASSERT(x, "invalid coordinate x = {}", x);
//     return cell_t(x, cell_length_, x_first_cell_);
//   }

//   /// Lies point \p x inside the grid?
//   constexpr bool contains(point_t x) const noexcept {
//     return geometry::contains(bounding_box_, x);
//   }

//   /// Coordinate of internal cell containing point \p x
//   constexpr coordinate_t internal_cell_containing(point_t x) const noexcept {
//     HM3_ASSERT(contains(x), "point x = {} lies outside the grid", x);

//     // normalize x:
//     auto x_min = x_first_cell_;
//     x() -= x_min();
//     x() /= cell_length();

//     typename cell_indices::tile_t::coordinate_t i;
//     for (suint_t d = 0; d < Nd; ++d) {
//       auto v = x[d];
//       i[d]   = std::lround(v);
//       HM3_ASSERT(i[d] >= cell_indices::first_internal()
//                   and i[d] <= cell_indices::last_internal(),
//                  "cell containing point {} lies outside the internal cells
//                  for "
//                  "dimension {}! value = {}, bounds = [{}, {}]",
//                  x, d, i[d], cell_indices::first_internal(),
//                  cell_indices::last_internal());
//     }
//     coordinate_t c(std::move(i));
//     HM3_ASSERT(c.is_internal(),
//                "internal cell {} containing point {} is not internal?", c,
//                x);
//     return c;
//   }

//   /// \name Construction
//   ///@{

//   constexpr void reinitialize(square_t bbox) noexcept {
//     bounding_box_ = std::move(bbox);
//     cell_length_  = compute_cell_length(geometry::length(bounding_box_));
//     x_first_cell_ = compute_first_cell_coordinates(bounding_box_);
//     HM3_ASSERT(cell_length() > 0.,
//                "zero cell length in square structured grid with bbox: {}",
//                bounding_box_);
//   }

//   constexpr grid()            = default;
//   constexpr grid(grid const&) = default;
//   constexpr grid& operator=(grid const&) = default;
//   constexpr grid(grid&&)                 = default;
//   constexpr grid& operator=(grid&&) = default;

//   constexpr grid(square_t bbox) { reinitialize(std::move(bbox)); }

//   ///@}
// };

}  // namespace structured
}  // namespace grid
}  // namespace hm3
