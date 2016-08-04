#pragma once
/// \file
///
/// Surface coordinate
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/structured/tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/surface/bounds.hpp>
#include <hm3/grid/structured/tile/surface/index.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace surface {

template <dim_t Nd, tidx_t Nc>  //
struct coordinate {
  using self            = coordinate;
  using cell_coordinate = cell::coordinate<Nd, Nc>;
  using point_t         = geometry::point<Nd>;
  using index           = index<Nd, Nc>;
  using bounds          = bounds<Nd, Nc>;
  using cell_bounds     = cell::bounds<Nd, Nc>;
  using value_t         = tidx_t;

  constexpr coordinate(cell_coordinate x_c_, suint_t d_) : x_c(x_c_), d(d_) {
    HM3_ASSERT(*this, "invalid surface with x_c: {}, d: {}", x_c, d);
  }

  constexpr coordinate(self const&) = default;
  constexpr coordinate(self&&)      = default;

  constexpr self& operator=(self const&) = default;
  constexpr self& operator=(self&&) = default;

  constexpr coordinate(index i) : coordinate(from(std::move(i))) {}

  /// Coordinate of a cell
  cell_coordinate x_c;
  /// Surface normal direction:
  ///
  ///        ____o____
  ///       |         |
  ///       |         |
  /// d = 0 o   x_c   o
  ///       |         |
  ///       |____o____|
  ///
  ///          d = 1
  ///
  /// For each x_c, only Nd surfaces are stored
  dim_t d;

  /// Surface normal vector.
  constexpr auto normal() const noexcept { return point_t::unit(d); }
  /// Coordinate of adjacent cell in negative normal direction
  constexpr cell_coordinate adjacent_neg() const noexcept {
    return x_c.offset(d, -1);
  }
  /// Coordinate of adjacent cell in positive normal direction
  constexpr cell_coordinate adjacent_pos() const noexcept { return x_c; }

  /// TODO: compute centroid in some normalized coordinate frame here?
  /// point_t centroid() const noexcept {}

  constexpr explicit operator bool() const noexcept {
    constexpr auto max_cell_idx = cell_bounds::length() + 1;
    // the cell index can be one row past the end of the tile on each component
    for (dim_t d_ = 0; d_ < Nd; ++d_) {
      const auto v = x_c[d_];
      static_assert(std::is_unsigned<value_t>{},
                    "for signed value_t it must also be checked whether v < 0 "
                    "(uncomment the check below)");
      if (v >= max_cell_idx /* || (std::is_signed<value_t>{} and v < 0) */) {
        return false;
      }
    }
    return d < Nd;
  }

  CONCEPT_REQUIRES(Nd == 1)
  static constexpr index idx(self x) noexcept {
    HM3_ASSERT(x, "invalid surface: {}", x);
    return index(x.x_c[0]);
  }

  CONCEPT_REQUIRES(Nd == 2)
  static constexpr index idx(self x) noexcept {
    HM3_ASSERT(x, "invalid surface: {}", x);
    if (x.d == 0) {
      return index(bounds::sides_per_dir() * x.x_c[1] + x.x_c[0]);
    }
    // d == 1:
    return index(bounds::size_per_dir() + bounds::sides_per_dir() * x.x_c[0]
                 + x.x_c[1]);
  }

  CONCEPT_REQUIRES(Nd == 3)
  static constexpr index idx(self x) noexcept {
    HM3_ASSERT(x, "invalid surface: {}", x);
    if (x.d == 0) {
      return index(bounds::sides_per_dir() * Nc * x.x_c[2]
                   + bounds::sides_per_dir() * x.x_c[1] + x.x_c[0]);
    }
    if (x.d == 1) {
      return index(bounds::size_per_dir()
                   + bounds::sides_per_dir() * (Nc * x.x_c[2] + x.x_c[0])
                   + x.x_c[1]);
    }
    // d == 2:
    return index(suint_t{2} * bounds::size_per_dir()
                 + bounds::sides_per_dir() * (Nc * x.x_c[1] + x.x_c[0])
                 + x.x_c[2]);
  }

  constexpr index idx() const noexcept { return idx(*this); }

  CONCEPT_REQUIRES(Nd == 1)
  static constexpr self from(index i) noexcept {
    HM3_ASSERT(i, "invalid index");
    self x(*i, suint_t{0});
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  CONCEPT_REQUIRES(Nd == 2)
  static constexpr self from(index id) noexcept {
    HM3_ASSERT(id, "invalid index");
    const auto v  = *id;
    const auto d_ = v / bounds::size_per_dir();
    HM3_ASSERT(d_ < Nd, "");
    auto x_c = d_ == 0? [=]() {
      const auto j   = v / bounds::sides_per_dir();
      const auto i   = v - j * bounds::sides_per_dir();
      return cell_coordinate(i, j);
    }() : [=]() { // d_ == 1;
      const auto v_ = v - bounds::size_per_dir();
      const auto i   = v_ / bounds::sides_per_dir();
      const auto j   = v_ - i * bounds::sides_per_dir();
      return cell_coordinate(i, j);
    }();
    self x(std::move(x_c), d_);
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", id, x);
    return x;
  }

  CONCEPT_REQUIRES(Nd == 3)
  static constexpr self from(index id) noexcept {
    HM3_ASSERT(id, "invalid index");
    const auto v  = *id;
    const auto d_ = v / bounds::size_per_dir();
    HM3_ASSERT(d_ < Nd, "");
    auto x_c = d_ == 0? [=]() {
      const auto k = v / (bounds::sides_per_dir() * Nc);
      const auto j   = (v - k * bounds::sides_per_dir() * Nc)
                       / bounds::sides_per_dir();
      const auto i   = v - k * bounds::sides_per_dir() * Nc
                       - j * bounds::sides_per_dir();
      return cell_coordinate(i, j, k);
    }() : d_== 1? [=](){
      const auto v_ = v - bounds::size_per_dir();
      const auto k = v_ / (bounds::sides_per_dir() * Nc);
      const auto i = (v_ - bounds::sides_per_dir() * Nc * k)
                      / bounds::sides_per_dir();
      const auto j = v_ - bounds::sides_per_dir() * (Nc * k + i);
      return cell_coordinate(i,j,k);
    }() : [=](){
       const auto v_ = v - 2 * bounds::size_per_dir();
       const auto j = v_ / (bounds::sides_per_dir() * Nc);
       const auto i = (v_ - bounds::sides_per_dir() * Nc * j)
                      / bounds::sides_per_dir();
       const auto k = v_ - bounds::sides_per_dir() * (Nc * j + i);
       return cell_coordinate(i,j,k);
    }();
    self x(std::move(x_c), d_);
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", id, x);
    return x;
  }

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream>
  friend OStream& operator<<(OStream& os, self const& ic) {
    if (ic) {
      os << "{" << ic.d << " : " << ic.x_c[0];
      for (dim_t d_ = 1; d_ < Nd; ++d_) { os << ", " << ic.x_c[d_]; }
      os << "}";
    } else {
      os << "{ invalid : invalid }";
    }
    return os;
  }

  constexpr friend bool operator==(self const& a, self const& b) noexcept {
    return a.idx() == b.idx();
  }
  constexpr friend bool operator!=(self const& a, self const& b) noexcept {
    return a.idx() != b.idx();
  }
  constexpr friend bool operator<(self const& a, self const& b) noexcept {
    return a.idx() < b.idx();
  }
  constexpr friend bool operator<=(self const& a, self const& b) noexcept {
    return a.idx() <= b.idx();
  }
  constexpr friend bool operator>(self const& a, self const& b) noexcept {
    return a.idx() > b.idx();
  }
  constexpr friend bool operator>=(self const& a, self const& b) noexcept {
    return a.idx() >= b.idx();
  }
};

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
