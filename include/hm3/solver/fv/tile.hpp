#pragma once
/// \file
///
/// Finite Volume Tile utilities
#include <hm3/grid/structured/grid.hpp>
#include <hm3/solver/utility/tile.hpp>
#include <hm3/solver/utility/tiled_hierarchical_grid.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Grid client type of FV state
template <typename Tile>  //
using grid_client_type = tiled_hierarchical_grid<Tile>;

/// Finite Volume tile layout
template <dim_t Nd, tidx_t Nic, tidx_t Nhl>
using tile_layout = grid::structured::grid<Nd, Nic, Nhl>;

template <typename T>  //
using tile_variables_t = typename T::tile_variables;

/// Helper to create a meta::list<Vars...> of tile variables
template <typename Grid, typename Physics, typename TimeIntegration,
          typename NumFlux, typename Method>
using tile_variables =                                             //
 meta::concat<                                                     //
  meta::invoke<tile_variables_t<Physics>, Grid>,                   //
  meta::invoke<tile_variables_t<TimeIntegration>, Grid, Physics>,  //
  meta::invoke<tile_variables_t<Method>, Grid, Physics, TimeIntegration,
               NumFlux>  //
  >;

/// This unwraps the variables from the meta::list<Vars...> into
/// `tile<Grid,Args...>`.
template <typename Arg, typename... Args>  //
struct unwrap_variables_into_tile {
  using type =
   // If you get this error it is most likely that you are passing the tile
   // variables as a pack, that is, `Vars...`, instead of in a
   // `meta::list<Vars...>`.
   typename Arg::ERROR_TILE_TYPE_TAKES_A_GRID_AND_A_META_LIST_OF_ARGUMENTS;
};

template <typename Grid, typename... Args>
struct unwrap_variables_into_tile<Grid, meta::list<Args...>> {
  using type = tile<Grid, Args...>;
};

/// Type of the tile
template <typename Grid, typename Physics, typename TimeIntegration,
          typename NumFlux, typename Method>
using tile_type = typename meta::
 _t<unwrap_variables_into_tile<Grid, tile_variables<Grid, Physics,
                                                    TimeIntegration, NumFlux,
                                                    Method>>>;  //

}  // namespace fv
}  // namespace solver
}  // namespace hm3

// tile.hpp
// #include <hm3/geometry/dimension.hpp>
// #include <hm3/geometry/primitive/point.hpp>
// #include <hm3/geometry/primitive/box.hpp>
// #include <hm3/grid/structured/grid.hpp>
// #include <hm3/solver/fv/cell_information.hpp>
// #include <hm3/solver/types.hpp>

// namespace hm3 {
// namespace solver {
// namespace fv {

// using tidx_t = grid::structured::tidx_t;

// template <dim_t Nd, uint_t Nv, tidx_t Nic, tidx_t Nhl>  //
// struct tile_base : grid::structured::grid<Nd, Nic, Nhl> {
//   using grid_t = grid::structured::grid<Nd, Nic, Nhl>;
//   using grid_t::dimension;

//   static constexpr auto size() { return grid_t::cells().size(); }

//   // todo: rename to cell_index_t ?
//   using index_t = typename grid_t::cell_indices_t::coordinate;

//   static constexpr uint_t no_variables() noexcept { return Nv; }
//   static constexpr uint_t nvars() noexcept { return Nv; }
//   static constexpr uint_t no_surfaces_per_cell() noexcept {
//     return 2 * dimension();
//   }
//   // static constexpr uint_t no_gradients() noexcept {
//   //   return dimension() * no_variables();
//   // }
//   static constexpr auto var_ids() noexcept {
//     return view::iota(0, no_variables());
//   }

//   static constexpr auto surfaces() noexcept {
//     return view::iota(0, no_surfaces_per_cell());
//   }

//   /// Data
//   ///@{
//   /// Tile length
//   num_t length;
//   /// Tile level
//   level_idx level;

//   /// \name Layout
//   ///
//   /// Cell variables, RHS, and Fluxes need to have the "same" memory layout
//   /// (i.e. the same column order) since the cell variables are updated from
//   /// the RHS "in bulk", and the RHS is updated from the fluxes "in bulk".
//   ///
//   ///@{
//   // Cell variables are in row_major_order (rows: cell, cols: variables)

//   using var_idx  = sidx_t;
//   using srfc_idx = sidx_t;
//   using dim_idx  = sidx_t;

//   // using cell_rhs_flux_order = dense::row_major_t;
//   using cell_rhs_flux_order = dense::col_major_t;

//   using var_t = dense::matrix<num_t, size(), no_variables(), sidx_t, var_idx,
//                               cell_rhs_flux_order>;

//   // using flux_t
//   //  = dense::matrix<num_t, size(), no_variables() * no_surfaces_per_cell(),
//   //                  sidx_t, suint_t, cell_rhs_flux_order>;

//   using gradient_t = dense::matrix<num_t, size(), no_variables() *
//   dimension(),
//                                    sidx_t, suint_t, cell_rhs_flux_order>;

//   ///@} Layout

//   /// Cell variables
//   var_t variables_;
//   /// Right Hand Side
//   var_t rhs_;
//   // /// Fluxes
//   // flux_t fluxes_;
//   /// Gradients
//   gradient_t gradients_;

//   // /// Cell information
//   // vector<cell_type> types_;

//   // /// Boundary cell data
//   // vector<boundary_cell> boundary_cells_;

//   ///@}  // Data

//   /// \name Cell variables
//   ///@{
//   auto& variables() noexcept { return variables_; }
//   auto const& variables() const noexcept { return variables_; }
//   auto variables(index_t c) noexcept { return variables_.row(*c.idx()); }
//   auto variables(index_t c) const noexcept { return variables_.row(*c.idx());
//   }
//   ///@}  Cell variables

//   /// \name Right hand side
//   ///@{
//   auto& rhs() noexcept { return rhs_; }
//   auto const& rhs() const noexcept { return rhs_; }
//   auto rhs(sidx_t c) noexcept { return rhs_.row(c); }
//   auto rhs(sidx_t c) const noexcept { return rhs_.row(c); }
//   auto rhs(index_t c) noexcept { return rhs(*c.idx()); }
//   auto rhs(index_t c) const noexcept { return rhs(*c.idx()); }
//   ///@} Right hand side

//   /// \name Gradients
//   ///@{
//   auto& gradients() noexcept { return gradients_; }
//   auto const& gradients() const noexcept { return gradients_; }
//   auto gradient(sidx_t c, dim_t d) noexcept {
//     return gradients_().row(c).template segment<nvars()>(d * nvars());
//   }
//   auto gradient(sidx_t c, dim_t d) const noexcept {
//     return gradients_().row(c).template segment<nvars()>(d * nvars());
//   }
//   auto gradient(index_t c, dim_t d) noexcept { return gradient(*c.idx(), d);
//   }
//   auto gradient(index_t c, dim_t d) const noexcept {
//     return gradient(*c.idx(), d);
//   }

//   ///@} Right hand side

//   // auto flux(sidx_t c, suint_t s) noexcept {
//   //   return fluxes_().row(c).template segment<no_variables()>(s
//   //                                                            *
//   // no_variables());
//   // }
//   // auto flux(sidx_t c, suint_t s) const noexcept {
//   //   return fluxes_().row(c).template segment<no_variables()>(s
//   //                                                            *
//   // no_variables());
//   // }

//   // auto flux(index_t c, suint_t s) noexcept { return flux(c.idx, s); }
//   // auto flux(index_t c, suint_t s) const noexcept { return flux(c.idx, s);
//   }

//   tile_base() {}

//   void reinitialize(level_idx level_, geometry::box<Nd> bbox) noexcept {
//     this->geometry().set_internal_bbox(bbox);
//     length = this->geometry().tile_internal_length();
//     level  = level;
//     HM3_ASSERT(this->geometry().tile_internal_bounding_box() == bbox, "");
//     HM3_ASSERT(*level_ >= 0, "negative tile level {}", level_);
//     HM3_ASSERT(length > 0., "zero tile length in tile with bbox: {}", bbox);
//     // info_.clear();
//     // info_.resize(size());
//   }

//   tile_base(level_idx level_, geometry::box<Nd> bbox)
//   // : info_(size()) {
//   {
//     reinitialize(std::move(level_), std::move(bbox));
//   }

//   void clear() {}

//  public:
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdeprecated"
//   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
// #pragma clang diagnostic pop
// };

// template <typename TileBase, typename TimeIntegrationState>  //
// struct tile : TileBase, TimeIntegrationState {
//   using TileBase::TileBase;
//   using TileBase::operator=;

//  public:
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdeprecated"
//   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
// #pragma clang diagnostic pop
// };

// }  // namespace fv
// }  // namespace solver
// }  // namespace hm3
