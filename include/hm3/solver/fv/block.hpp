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

template <uint_t Nd, uint_t Nv, uint_t Nic, uint_t Nhl>  //
struct block_base : square_structured_grid<Nd, Nic, Nhl> {
  static_assert(Nic >= Nhl, "");
  using grid_t = square_structured_grid<Nd, Nic, Nhl>;
  using grid_t::dimension;
  using grid_t::size;
  using index_t = typename grid_t::index_t;

  static constexpr uint_t no_variables() noexcept { return Nv; }
  static constexpr uint_t nvars() noexcept { return Nv; }
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

  using cell_rhs_flux_order = dense::row_major_t;

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

  TimeIntegration time_int;

  ///@}  // Data

  /// \name Cell variables
  ///@{
  auto& variables() noexcept { return variables_; }
  auto const& variables() const noexcept { return variables_; }
  auto variables(sidx_t c) noexcept { return variables_().row(c); }
  auto variables(sidx_t c) const noexcept { return variables_().row(c); }
  auto variables(index_t c) noexcept { return variables(c.idx); }
  auto variables(index_t c) const noexcept { return variables(c.idx); }
  ///@}  Cell variables

  /// \name Right hand side
  ///@{
  auto& rhs() noexcept { return rhs_; }
  auto const& rhs() const noexcept { return rhs_; }
  auto rhs(sidx_t c) noexcept { return rhs_.row(c); }
  auto rhs(sidx_t c) const noexcept { return rhs_.row(c); }
  auto rhs(index_t c) noexcept { return rhs(c.idx); }
  auto rhs(index_t c) const noexcept { return rhs(c.idx); }
  ///@} Right hand side

  /// \name Gradients
  ///@{
  auto& gradients() noexcept { return gradients_; }
  auto const& gradients() const noexcept { return gradients_; }
  auto gradient(sidx_t c, suint_t d) noexcept {
    return gradients_().row(c).template segment<nvars()>(d * nvars());
  }
  auto gradient(sidx_t c, suint_t d) const noexcept {
    return gradients_().row(c).template segment<nvars()>(d * nvars());
  }
  auto gradient(index_t c, suint_t d) noexcept { return gradient(c.idx, d); }
  auto gradient(index_t c, suint_t d) const noexcept {
    return gradient(c.idx, d);
  }

  ///@} Right hand side

  auto flux(sidx_t c, suint_t s) noexcept {
    return fluxes_().row(c).template segment<no_variables()>(s
                                                             * no_variables());
  }
  auto flux(sidx_t c, suint_t s) const noexcept {
    return fluxes_().row(c).template segment<no_variables()>(s
                                                             * no_variables());
  }

  auto flux(index_t c, suint_t s) noexcept { return flux(c.idx, s); }
  auto flux(index_t c, suint_t s) const noexcept { return flux(c.idx, s); }

  block() {}

  void reinitialize(level_idx block_level_,
                    geometry::square<Nd> bbox) noexcept {
    grid_t::reinitialize(bbox);
    block_length = geometry::length(bbox);
    block_level  = block_level;
    HM3_ASSERT(*block_level_ >= 0, "negative block level {}", block_level_);
    HM3_ASSERT(block_length > 0., "zero block length in block with bbox: {}",
               bbox);
  }

  block(level_idx block_level_, geometry::square<Nd> bbox) {
    reinitialize(std::move(block_level_), std::move(bbox));
  }

  void clear() {}

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
};

template <typename BlockBase, typename TimeIntegrationState>  //
struct block : BlockBase, TimeIntegrationState {
  using BlockBase::BlockBase;
  using BlockBase::operator=;

 public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#pragma clang diagnostic pop
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
