#pragma once
/// \file block.hpp
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/solver/types.hpp>
#include <hm3/solver/utility/structured_grid.hpp>

namespace hm3 {
namespace solver {
namespace fv {

// template <typename T, suint_t length> struct view {
//   HM3_RESTRICT T* data;
//   T& operator[](suint_t i) noexcept {
//     HM3_ASSERT(i < length, "index {} is out of bounds[0, {})", i, length);
//     return data[i];
//   }
//   T const& operator[](suint_t i) const noexcept {
//     HM3_ASSERT(i < length, "index {} is out of bounds[0, {})", i, length);
//     return data[i];
//   }
//   static constexpr suint_t size() noexcept { return length; }
// };

template <uint_t Nd, uint_t Nv, uint_t Nic, uint_t Nhl>  //
struct block : square_structured_grid<Nd, Nic, Nhl> {
  static_assert(Nic >= Nhl, "");
  using grid_t = square_structured_grid<Nd, Nic, Nhl>;
  using grid_t::dimension;
  using grid_t::size;
  using index_t = typename grid_t::index_t;

  static constexpr uint_t no_variables() noexcept { return Nv; }
  static constexpr uint_t no_surfaces_per_cell() noexcept {
    return 2 * dimension();
  }
  // static constexpr uint_t no_gradients() noexcept {
  //   return dimension() * no_variables();
  // }
  static constexpr auto var_ids() noexcept {
    return view::iota(0, no_variables());
  }

  static constexpr auto surfaces() noexcept {
    return view::iota(0, no_surfaces_per_cell());
  }

  /// Data
  ///@{
  /// Block length
  num_t block_length;
  /// Block level
  level_idx block_level;

  /// \name Layout
  ///
  /// Cell variables, RHS, and Fluxes need to have the "same" memory layout
  /// (i.e. the same column order) since the cell variables are updated from
  /// the RHS "in bulk", and the RHS is updated from the fluxes "in bulk".
  ///
  ///@{
  // Cell variables are in row_major_order (rows: cell, cols: variables)

  using var_idx  = sidx_t;
  using srfc_idx = sidx_t;
  using dim_idx  = sidx_t;

  using cell_rhs_flux_order = dense::col_major_t;

  using var_t = dense::matrix<num_t, size(), no_variables(), sidx_t, var_idx,
                              cell_rhs_flux_order>;

  using flux_t
   = dense::matrix<num_t, size(), no_variables() * no_surfaces_per_cell(),
                   sidx_t, suint_t, cell_rhs_flux_order>;

  using gradient_t = dense::matrix<num_t, size(), no_variables() * dimension(),
                                   sidx_t, suint_t, cell_rhs_flux_order>;

  ///@} Layout

  /// Cell variables
  var_t variables_;
  /// Right Hand Side
  var_t rhs_;
  /// Fluxes
  flux_t fluxes_;
  /// Gradients
  gradient_t gradients_;

  ///@}  // Data

  decltype(auto) variables(sidx_t c) noexcept { return variables_().row(c); }
  decltype(auto) variables(sidx_t c) const noexcept {
    return variables_().row(c);
  }

  decltype(auto) variables(index_t c) noexcept { return variables(c.idx); }
  decltype(auto) variables(index_t c) const noexcept {
    return variables(c.idx);
  }

  decltype(auto) rhs(sidx_t c) noexcept { return rhs_().row(c); }
  decltype(auto) rhs(sidx_t c) const noexcept { return rhs_().row(c); }

  decltype(auto) rhs(index_t c) noexcept { return rhs(c.idx); }
  decltype(auto) rhs(index_t c) const noexcept { return rhs(c.idx); }

  decltype(auto) flux(sidx_t c, suint_t s) noexcept {
    return fluxes_().row(c).template segment<no_variables()>(s
                                                             * no_variables());
  }
  decltype(auto) flux(sidx_t c, suint_t s) const noexcept {
    return fluxes_().row(c).template segment<no_variables()>(s
                                                             * no_variables());
  }

  decltype(auto) flux(index_t c, suint_t s) noexcept { return flux(c.idx, s); }
  decltype(auto) flux(index_t c, suint_t s) const noexcept {
    return flux(c.idx, s);
  }

  decltype(auto) gradient(sidx_t c, suint_t d) noexcept {
    return gradients_().row(c).template segment<no_variables()>(
     d * no_variables());
  }
  decltype(auto) gradient(sidx_t c, suint_t d) const noexcept {
    return gradients_().row(c).template segment<no_variables()>(
     d * no_variables());
  }

  decltype(auto) gradient(index_t c, suint_t d) noexcept {
    return gradient(c.idx, d);
  }
  decltype(auto) gradient(index_t c, suint_t d) const noexcept {
    return gradient(c.idx, d);
  }

  block() {}

  block(level_idx block_level_, geometry::square<Nd> bbox)
   : square_structured_grid<Nd, Nic, Nhl>(bbox)
   , block_length(geometry::length(bbox))
   , block_level(block_level_) {
    HM3_ASSERT(*block_level_ >= 0, "negative block level {}", block_level_);
    HM3_ASSERT(block_length > 0., "zero block length in block with bbox: {}",
               bbox);
  }

  // struct cell_t : grid_t::cell_t {
  //   using base_t = grid_t::cell_t;
  //   using base_t::idx;
  //   using base_t::grid;

  //   cell_t(base_t c) : base_t{std::move(c)} {}

  //   num_t& v(uint_t i) noexcept { return grid.variables() * (vars_ + i); }
  //   num_t const& v(uint_t i) const noexcept { return *(vars_ + i); }
  //   num_t& g(uint_t i, uint_t d) noexcept { return *(grads_ + Nd * i + d); }
  //   num_t const& g(uint_t i, uint_t d) const noexcept {
  //     return *(grads_ + Nd * i + d);
  //   }
  //   num_t& r(uint_t i) noexcept { return *(rhs_ + i); }
  //   num_t const& r(uint_t i) const noexcept { return *(rhs_ + i); }
  //   num_t& f(uint_t s, uint_t i) noexcept { return *(fluxes_ + s * Nv + i); }
  //   num_t const& f(uint_t s, uint_t i) const noexcept {
  //     return *(fluxes_ + s * Nv + i);
  //   }

  //   friend bool operator==(cell_t const& a, cell_t const& b) {
  //     return static_cast<base_t const&>(a) == static_cast<base_t const & b>;
  //   }
  //   friend bool operator!=(cell_t const& a, cell_t const& b) {
  //     return !(a == b);
  //   }
  // };
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
