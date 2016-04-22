#pragma once
/// \file
///
/// Square tile cell indexed coordinate
#include <hm3/grid/structured/tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/cell/index.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace cell {

/// Square tile cell index-coordinate pair
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length
template <dim_t Nd, tidx_t Nc>
struct indexed_coordinate : geometry::dimensional<Nd> {
  using self           = indexed_coordinate;
  using index          = index<Nd, Nc>;
  using coordinate     = coordinate<Nd, Nc>;
  using offset_t       = typename coordinate::offset_t;
  using value_t        = typename coordinate::value_t;
  using signed_value_t = typename coordinate::signed_value_t;
  index idx_;
  coordinate x_;

  constexpr indexed_coordinate() : idx_(index::invalid()), x_() {}

  constexpr indexed_coordinate(index idx, coordinate x)
   : idx_(std::move(idx)), x_(std::move(x)) {
    HM3_ASSERT(coordinate(this->idx()) == this->x(),
               "idx {} produces x {} but x is {}", this->idx(), coordinate(idx),
               this->x());
    HM3_ASSERT(this->x().idx() == this->idx(),
               "x {} produces idx {} but idx is {}", this->x(), this->x().idx(),
               this->idx());
  }
  constexpr indexed_coordinate(index idx) : idx_(idx), x_(std::move(idx)) {}
  constexpr indexed_coordinate(coordinate x)
   : idx_(coordinate::idx(x)), x_(std::move(x)) {}

  constexpr explicit operator bool() const noexcept {
    HM3_ASSERT(static_cast<bool>(idx()) == static_cast<bool>(x()),
               "idx {} to bool is {}, but coordinate {} to bool is {}", idx(),
               static_cast<bool>(idx()), x(), static_cast<bool>(x()));
    return static_cast<bool>(idx());
  }

  constexpr index idx() const noexcept { return idx_; }
  constexpr coordinate x() const noexcept { return x_; }

  constexpr explicit operator index() const noexcept { return idx(); }
  constexpr explicit operator coordinate() const noexcept { return x(); }

  /// \p d -th coordinate component
  constexpr auto operator[](dim_t d) const noexcept {
    HM3_ASSERT(x(), "invalid coordinate {}", x());
    return x()[d];
  }
  /// Coordinate index
  constexpr auto operator*() const noexcept {
    HM3_ASSERT(idx(), "coordinate {} has invalid idx {}", x(), idx().value);
    return *idx();
  }

  /// Offset coordinate \p d -th component by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(dim_t d, noffset_t o) const noexcept {
    coordinate n = x().offset(d, o);
    HM3_ASSERT(n, "offsetting x = {} by ({}, {}) results in invalid coordinate",
               x(), d, o);
    return {n};
  }

  /// Offset coordinate by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(offset_t o) const noexcept {
    coordinate n = x().offset(o);
    HM3_ASSERT(n, "offsetting x = {} by {} results in invalid coordinate", x(),
               o);
    return {n};
  }

  /// Coordinate at \p offset along the \p d -th component from this
  ///
  /// Returns invalid if the result lies outside the tile.
  constexpr self at(dim_t d, noffset_t offset) const noexcept {
    coordinate n = x().offset(d, offset);
    return n ? self{n} : self{index(), n};
  }

  /// Coordinate at \p offset from this
  ///
  /// Returns invalid if the result lies outside the tile.
  constexpr self at(offset_t offset) const noexcept {
    coordinate n = x().offset(offset);
    return n ? self{n} : self{index(), n};
  }

  /// Constructor from constant value
  static constexpr self constant(value_t i) noexcept {
    return coordinate::constant(i);
  }

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream>
  friend OStream& operator<<(OStream& os, self const& ic) {
    if (ic) {
      os << "{" << ic.idx() << " : " << ic.x()[0];
      for (dim_t d = 1; d < Nd; ++d) { os << ", " << ic.x()[d]; }
      os << "}";
    } else {
      os << "{ invalid : invalid }";
    }
    return os;
  }
};

}  // namespace cell
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
