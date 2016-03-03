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
template <suint_t Nd, suint_t Nc>
struct indexed_coordinate : geometry::dimensional<Nd> {
  using self           = indexed_coordinate;
  using index          = index<Nd, Nc>;
  using coordinate     = coordinate<Nd, Nc>;
  using offset_t       = typename coordinate::offset_t;
  using value_t        = typename coordinate::value_t;
  using signed_value_t = typename coordinate::signed_value_t;
  index idx;
  coordinate x;

  constexpr indexed_coordinate() : idx(index::invalid()), x() {}

  constexpr indexed_coordinate(index idx_, coordinate x_)
   : idx(std::move(idx_)), x(std::move(x_)) {
    HM3_ASSERT(coordinate(idx) == x, "idx {} produces x {} but x is {}", idx,
               coordinate(idx), x);
    HM3_ASSERT(x.idx() == idx, "x {} produces idx {} but idx is {}", x, x.idx(),
               idx);
  }
  constexpr indexed_coordinate(index idx_) : idx(idx_), x(std::move(idx_)) {}
  constexpr indexed_coordinate(coordinate x_)
   : idx(coordinate::idx(x_)), x(std::move(x_)) {}

  constexpr explicit operator bool() const noexcept {
    HM3_ASSERT(static_cast<bool>(idx) == static_cast<bool>(x),
               "idx {} to bool is {}, but coordinate {} to bool is {}", idx,
               static_cast<bool>(idx), x, static_cast<bool>(x));
    return static_cast<bool>(idx);
  }

  constexpr explicit operator index() const noexcept { return idx; }
  constexpr explicit operator coordinate() const noexcept { return x; }

  /// \p d -th coordinate component
  constexpr auto operator[](suint_t d) const noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    return x[d];
  }
  /// Coordinate index
  constexpr auto operator*() const noexcept {
    HM3_ASSERT(idx, "coordinate {} has invalid idx {}", x, idx.value);
    return *idx;
  }

  /// Offset coordinate \p d -th component by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(suint_t d, sint_t o) const noexcept {
    coordinate n = x.offset(d, o);
    HM3_ASSERT(n, "offsetting x = {} by ({}, {}) results in invalid coordinate",
               x, d, o);
    return {n};
  }

  /// Offset coordinate by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(offset_t o) const noexcept {
    coordinate n = x.offset(o);
    HM3_ASSERT(n, "offsetting x = {} by {} results in invalid coordinate", x,
               o);
    return {n};
  }

  /// Coordinate at \p offset along the \p d -th component from this
  ///
  /// Returns invalid if the result lies outside the tile.
  constexpr self at(suint_t d, sint_t offset) const noexcept {
    coordinate n = x.offset(d, offset);
    return n ? self{n} : self{index(), n};
  }

  /// Coordinate at \p offset from this
  ///
  /// Returns invalid if the result lies outside the tile.
  constexpr self at(offset_t offset) const noexcept {
    coordinate n = x.offset(offset);
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
      os << "{" << ic.idx << " : " << ic.x[0];
      for (suint_t d = 1; d < Nd; ++d) { os << ", " << ic.x[d]; }
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
