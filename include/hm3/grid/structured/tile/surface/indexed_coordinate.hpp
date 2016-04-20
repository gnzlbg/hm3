 #pragma once
/// \file
///
/// Square tile surface indexed coordinate
#include <hm3/grid/structured/tile/surface/coordinate.hpp>
#include <hm3/grid/structured/tile/surface/index.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace surface {

/// Square tile surface index-coordinate pair
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of surfaces per tile length
template <suint_t Nd, suint_t Nc>
struct indexed_coordinate : geometry::dimensional<Nd> {
  using self           = indexed_coordinate;
  using index          = index<Nd, Nc>;
  using coordinate     = coordinate<Nd, Nc>;
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

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream>
  friend OStream& operator<<(OStream& os, self const& ic) {
    if (ic) {
      os << "{" << ic.idx << " : " << ic.x_c[0];
      for (suint_t d = 1; d < Nd; ++d_) { os << ", " << ic.x[d]; }
      os << "}";
    } else {
      os << "{ invalid : invalid }";
    }
    return os;
  }
};

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
