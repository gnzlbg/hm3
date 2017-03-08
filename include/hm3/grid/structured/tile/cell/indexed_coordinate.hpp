#pragma once
/// \file
///
/// Square tile cell indexed coordinate
#include <hm3/grid/structured/tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/cell/index.hpp>

namespace hm3::grid::structured::tile::cell {

/// Square tile cell index-coordinate pair
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc number of cells per tile length
template <dim_t Ad, tidx_t Nc>
struct indexed_coordinate : public coordinate<Ad, Nc> {
  using self            = indexed_coordinate;
  using index           = index<Ad, Nc>;
  using coordinate      = coordinate<Ad, Nc>;
  using offset_t        = typename coordinate::offset_t;
  using value_t         = typename coordinate::value_t;
  using signed_value_t  = typename coordinate::signed_value_t;
  using difference_type = std::ptrdiff_t;

  // coordinate x_;
  using x_ = coordinate;
  index idx_;

  static void assert_invariants(self x) noexcept {
    // checks that both convert to the same boolean value, both are either valid
    // or invalid, and also that the index and coordinate types match:
    HM3_ASSERT(static_cast<bool>(x.idx()) == static_cast<bool>(x.x()),
               "idx {} to bool is {}, but coordinate {} to bool is {}", x.idx(),
               static_cast<bool>(x.idx()), x.x(), static_cast<bool>(x.x()));
    HM3_ASSERT((not x.idx() and not x.x()) or (x.idx() and x.x()),
               "both must be valid/invalid");
    if (not x.idx()) { return; }
    HM3_ASSERT(coordinate(x.idx()) == x.x(), "idx {} produces x {} but x is {}",
               x.idx(), coordinate(x.idx()), x.x());
    HM3_ASSERT(x.x().idx() == x.idx(), "x {} produces idx {} but idx is {}",
               x.x(), x.x().idx(), x.idx());
  }

  void assert_invariants() const noexcept { assert_invariants(*this); }

  constexpr indexed_coordinate() : x_(), idx_(index::invalid()) {
    assert_invariants();
  }

  constexpr indexed_coordinate(index idx, coordinate x)
   : x_(std::move(x)), idx_(std::move(idx)) {
    assert_invariants();
  }
  constexpr indexed_coordinate(index idx) : x_(idx), idx_(std::move(idx)) {}
  constexpr indexed_coordinate(coordinate x) : x_(x), idx_(coordinate::idx(x)) {
    assert_invariants();
  }

  constexpr explicit operator bool() const noexcept {
    assert_invariants();
    return static_cast<bool>(idx());
  }

  constexpr index idx() const noexcept { return idx_; }
  constexpr coordinate x() const noexcept {
    return static_cast<coordinate const&>(*this);
  }

  constexpr operator index() const noexcept {
    assert_invariants();
    return idx();
  }
  constexpr operator coordinate() const noexcept {
    assert_invariants();
    return x();
  }

  /// \p d -th coordinate component
  constexpr auto operator[](dim_t d) const noexcept {
    HM3_ASSERT(x(), "invalid coordinate {}", x());
    assert_invariants();
    return x()[d];
  }
  /// Coordinate index
  constexpr auto operator*() const noexcept {
    HM3_ASSERT(idx(), "coordinate {} has invalid idx {}", x(), idx().value);
    assert_invariants();
    return *idx();
  }

  /// Offset coordinate \p d -th component by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(dim_t d, noffset_t o) const noexcept {
    assert_invariants();
    coordinate n = x().offset(d, o);
    HM3_ASSERT(n, "offsetting x = {} by ({}, {}) results in invalid coordinate",
               x(), d, o);
    assert_invariants(self(n));
    return {n};
  }

  /// Offset coordinate by \p o
  ///
  /// \pre The resulting coordinate must lie within the tile
  /// \note Faster than `at`
  constexpr self offset(offset_t o) const noexcept {
    assert_invariants();
    coordinate n = x().offset(o);
    HM3_ASSERT(n, "offsetting x = {} by {} results in invalid coordinate", x(),
               o);
    assert_invariants(self(n));
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
    assert_invariants();
    coordinate n = x().offset(offset);
    return n ? self{n} : self{index(), n};
  }

  /// Constructor from constant value
  static constexpr self constant(value_t i) noexcept {
    return coordinate::constant(i);
  }

  constexpr self& operator++() noexcept {
    assert_invariants();
    ++idx_;
    static_cast<coordinate&>(*this) = coordinate::from(idx_);
    assert_invariants();
    return (*this);
  }

  constexpr self operator++(int)noexcept {
    assert_invariants();
    self tmp(*this);
    ++(*this);
    assert_invariants();
    return tmp;
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

  friend constexpr bool operator==(indexed_coordinate const& a,
                                   indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return a.idx() == b.idx();
  }
  friend constexpr bool operator!=(indexed_coordinate const& a,
                                   indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return not(a == b);
  }
  friend constexpr bool operator<(indexed_coordinate const& a,
                                  indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return a.idx() < b.idx();
  }
  friend constexpr bool operator<=(indexed_coordinate const& a,
                                   indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return a.idx() <= b.idx();
  }

  friend constexpr bool operator>(indexed_coordinate const& a,
                                  indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return a.idx() > b.idx();
  }

  friend constexpr bool operator>=(indexed_coordinate const& a,
                                   indexed_coordinate const& b) noexcept {
    assert_invariants(a);
    assert_invariants(b);
    return a.idx() >= b.idx();
  }
};

}  // namespace hm3::grid::structured::tile::cell

namespace std {

template <hm3::dim_t Ad, hm3::grid::structured::tile::tidx_t Nc>
struct hash<hm3::grid::structured::tile::cell::indexed_coordinate<Ad, Nc>> {
  constexpr std::size_t operator()(
   hm3::grid::structured::tile::cell::indexed_coordinate<Ad, Nc> const& c) const
   noexcept {
    return static_cast<std::size_t>(*(c.idx_));
  }
};

}  // namespace std
