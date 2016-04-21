#pragma once
/// \file
#include <hm3/solver/utility/structured_grid.hpp>

namespace hm3 {
namespace solver {
namespace lbm {

template <dim_t Nd, uint_t Ndist, uint_t Nic, uint_t Nhl>
struct block : square_structured_grid<Nd, Nic, Nhl> {
  using grid_t = square_structured_grid<Nd, Nic, Nhl>;
  using grid_t::dimension;
  using grid_t::size;
  using index_t = typename grid_t::index_t;

  static constexpr uint_t distribution_size() noexcept {
    return size() * Ndist;
  }

  level_idx level_;
  constexpr level_idx level() const noexcept { return level_; }

  std::array<num_t, distribution_size()> nodes_0;
  std::array<num_t, distribution_size()> nodes_1;
  std::array<bool, size()> is_boundary_;

  static constexpr uint_t dist(sidx_t c, uint_t d) noexcept {
    return Ndist * c + d;
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

  void reinitialize(level_idx block_level_,
                    geometry::square<Nd> bbox) noexcept {
    grid_t::reinitialize(bbox);
    level_ = block_level_;
  }

  block(level_idx l, geometry::square<Nd> bbox) noexcept {
    reinitialize(l, bbox);
  }
};

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
