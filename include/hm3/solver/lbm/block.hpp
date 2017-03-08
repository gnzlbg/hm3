#ifdef FIXED

#pragma once
/// \file
#include <hm3/solver/utility/structured_grid.hpp>

namespace hm3::solver::lbm {

template <dim_t Ad, uint_t Adist, uint_t Nic, uint_t Nhl>
struct block : square_structured_grid<Ad, Nic, Nhl> {
  using grid_t = square_structured_grid<Ad, Nic, Nhl>;
  using grid_t::ambient_dimension;
  using grid_t::size;
  using index_t = typename grid_t::index_t;

  static constexpr uint_t distribution_size() noexcept {
    return size() * Adist;
  }

  level_idx level_;
  constexpr level_idx level() const noexcept { return level_; }

  array<num_t, distribution_size()> nodes_0;
  array<num_t, distribution_size()> nodes_1;
  array<bool, size()> is_boundary_;

  static constexpr uint_t dist(sidx_t c, uint_t d) noexcept {
    return Adist * c + d;
  }

  num_t const& nodes0(index_t c, uint_t d) const noexcept {
    return nodes_0[dist(c.idx, d)];
  }
  num_t& nodes0(index_t c, uint_t d) noexcept {
    return nodes_0[dist(c.idx, d)];
  }
  num_t const& nodes1(index_t c, uint_t d) const noexcept {
    return nodes_1[dist(c.idx, d)];
  }
  num_t& nodes1(index_t c, uint_t d) noexcept {
    return nodes_1[dist(c.idx, d)];
  }
  num_t const& nodes0(sidx_t c, uint_t d) const noexcept {
    return nodes_0[dist(c, d)];
  }
  num_t& nodes0(sidx_t c, uint_t d) noexcept { return nodes_0[dist(c, d)]; }
  num_t const& nodes1(sidx_t c, uint_t d) const noexcept {
    return nodes_1[dist(c, d)];
  }
  num_t& nodes1(sidx_t c, uint_t d) noexcept { return nodes_1[dist(c, d)]; }

  bool const& is_boundary(index_t c) const noexcept {
    return is_boundary_[c.idx];
  }
  bool& is_boundary(index_t c) noexcept { return is_boundary_[c.idx]; }

  block() = default;

  void reinitialize(level_idx block_level_, geometry::box<Ad> bbox) noexcept {
    grid_t::reinitialize(bbox);
    level_ = block_level_;
  }

  block(level_idx l, geometry::box<Ad> bbox) noexcept { reinitialize(l, bbox); }
};

}  // namespace hm3::solver::lbm
#endif
