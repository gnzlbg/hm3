#pragma once
/// \file
///
/// Square tile surface indexed coordinate
#include <hm3/grid/structured/tile/surface/coordinate.hpp>
#include <hm3/grid/structured/tile/surface/index.hpp>

namespace hm3::grid::structured::tile::surface {

/// Square tile surface index-coordinate pair
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc number of surfaces per tile length
template <dim_t Ad, tidx_t Nc>
struct indexed_coordinate : geometry::with_ambient_dimension<Ad> {
  using self       = indexed_coordinate;
  using index      = index<Ad, Nc>;
  using coordinate = coordinate<Ad, Nc>;
  index idx_;
  coordinate x_;

  constexpr indexed_coordinate() : idx_(index::invalid()), x_() {}

  constexpr indexed_coordinate(index idx, coordinate x)
   : idx_(std::move(idx)), x_(std::move(x)) {
    HM3_ASSERT(coordinate(idx()) == x(), "idx {} produces x {} but x is {}",
               idx(), coordinate(idx()), x());
    HM3_ASSERT(x().idx() == idx(), "x {} produces idx {} but idx is {}", x(),
               x().idx(), idx());
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

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream>
  friend OStream& operator<<(OStream& os, self const& ic) {
    if (ic) {
      os << "{" << ic.idx() << " : " << ic.x()[0];
      for (dim_t d = 1; d < Ad; ++d) { os << ", " << ic.x()[d]; }
      os << "}";
    } else {
      os << "{ invalid : invalid }";
    }
    return os;
  }
};

}  // namespace hm3::grid::structured::tile::surface
