#pragma once
/// \file
///
/// Square tile coordinate
#include <hm3/geometry/dimensions.hpp>
#include <hm3/grid/structured/tile/bounds.hpp>
#include <hm3/grid/structured/tile/index.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/utility/config/attributes.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace structured {
namespace square {
namespace tile {

/// Square tile cell coordinate
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per length
template <suint_t Nd, suint_t Nc>
struct coordinate : geometry::dimensional<Nd> {
  using value_t        = index_type;
  using signed_value_t = std::make_signed_t<value_t>;
  using coordinates_t  = std::array<value_t, Nd>;
  using offset_t       = std::array<signed_value_t, Nd>;
  using index          = index<Nd, Nc>;
  using bounds         = bounds<Nd, Nc>;
  static_assert(sizeof(value_t) == sizeof(signed_value_t), "");

  coordinates_t xs;

  /// \name Array of invalid coordinates
  ///@{
  static constexpr value_t invalid_x_() noexcept {
    return std::numeric_limits<value_t>::max();
  }
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr coordinates_t invalid_xs_() noexcept {
    return {{invalid_x_()}};
  }
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr coordinates_t invalid_xs_() noexcept {
    return {{invalid_x_(), invalid_x_()}};
  }
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr coordinates_t invalid_xs_() noexcept {
    return {{invalid_x_(), invalid_x_(), invalid_x_()}};
  }
  ///@} // Array of invalid coordinates

  constexpr coordinate() : xs{invalid_xs_()} {}
  constexpr coordinate(coordinate const&) = default;
  constexpr coordinate(coordinate&&)      = default;

  constexpr coordinate& operator=(coordinate const&) = default;
  constexpr coordinate& operator=(coordinate&&) = default;

  /// Constructs a coordinate from the index
  constexpr coordinate(index i) noexcept : coordinate(from(std::move(i))) {}

  /// \name Constructors from coordinate indices {i,j,k}
  ///@{

  CONCEPT_REQUIRES(Nd == 1)
  constexpr coordinate(value_t i) noexcept : xs{{std::move(i)}} {}

  CONCEPT_REQUIRES(Nd == 2)
  constexpr coordinate(value_t i, value_t j) noexcept
   : xs{{std::move(i), std::move(j)}} {}

  CONCEPT_REQUIRES(Nd == 3)
  constexpr coordinate(value_t i, value_t j, value_t k) noexcept
   : xs{{std::move(i), std::move(j), std::move(k)}} {}

  ///@}  // Constructors from coordinate indices {i,j,k}

  /// \name Constructors from constant value
  ///@{
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr coordinate constant(value_t v) noexcept {
    return coordinate(v);
  }

  CONCEPT_REQUIRES(Nd == 2)
  static constexpr coordinate constant(value_t v) noexcept {
    return coordinate(v, v);
  }

  CONCEPT_REQUIRES(Nd == 3)
  static constexpr coordinate constant(value_t v) noexcept {
    return coordinate(v, v, v);
  }

  ///@}  // Constructors from constant value

  /// Access \p d -th coordinate component
  constexpr value_t operator[](value_t d) const noexcept {
    HM3_ASSERT(d < Nd, "index {} is out-of-bounds [0, {})", d, Nd);
    return xs[d];
  }
  /// Access \p d -th coordinate component
  constexpr value_t& operator[](value_t d) noexcept {
    HM3_ASSERT(d < Nd, "index {} is out-of-bounds [0, {})", d, Nd);
    return xs[d];
  }

  /// Returns true if the coordinate is within the tile bounds, false
  /// otherwise
  constexpr explicit operator bool() const noexcept {
    constexpr auto cpl = bounds::length();
    for (value_t d = 0; d < Nd; ++d) {
      const auto v = xs[d];
      static_assert(std::is_unsigned<value_t>{},
                    "for signed value_t it must also be checked whether v < 0 "
                    "(uncomment the check below)");
      if (v >= cpl /* || (std::is_signed<value_t>{} and v < 0) */) {
        return false;
      }
    }
    return true;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr coordinate from(index i) noexcept {
    HM3_ASSERT(i, "invalid index");
    coordinate x(*i);
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr coordinate from(index i) noexcept {
    HM3_ASSERT(i, "invalid index");
    constexpr auto cpl = bounds::length();
    const auto iv      = *i;
    coordinate x(iv % cpl, iv / cpl);
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr coordinate from(index i) noexcept {
    HM3_ASSERT(i, "invalid index");
    constexpr auto cpl = bounds::length();
    const auto iv      = *i;
    const auto tmp     = iv / cpl;
    coordinate x(iv % cpl, tmp % cpl, tmp / cpl);
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the
  /// grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 1)
  [[HM3_FLATTEN]] static constexpr index idx(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    index i(x[0]);
    return i;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the
  /// grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 2)
  [[HM3_FLATTEN]] static constexpr index idx(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    index i(x[0] + bounds::length() * x[1]);
    return i;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the
  /// grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 3)
  [[HM3_FLATTEN]] static constexpr index idx(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    constexpr auto cpl = bounds::length();
    index i(x[0] + cpl * x[1] + cpl * cpl * x[2]);
    return i;
  }

  /// Index corresponding to coordinate
  [[HM3_FLATTEN]] constexpr index idx() const noexcept { return idx(*this); }

  /// Explicit conversion operator to index
  [[HM3_FLATTEN]] constexpr explicit operator index() const noexcept {
    return idx(*this);
  }

  /// Offset coordinate \p d -th component by \p o
  ///
  /// If the coordinate is outside the tile it will be invalid.
  [[clang::no_sanitize("integer")]] constexpr coordinate offset(
   value_t d, signed_value_t o) const noexcept {
    coordinate n          = (*this);
    signed_value_t new_xs = static_cast<signed_value_t>(xs[d]) + o;
    // If the new xs is negative the assignment will wrap around. This
    // should always result in an invalid tile, but doing this right would
    // require something like:
    //
    // n[d] = new_xs >= 0? new_xs : std::numeric_limits<value_t>::max();
    //
    // which introduces a branch. So instead we just rely on the fact that
    // tiles should be small and wrapping unsigned arithmetic:
    n[d] = static_cast<value_t>(new_xs);
    HM3_ASSERT(
     new_xs >= 0 || !n,
     "negative coordinate xs does not result in an invalid coordinate");
    return n;
  }

  /// Offset coordinate by \p o
  ///
  /// If the coordinate is outside the tile it will be invalid.
  [[clang::no_sanitize("integer")]] constexpr coordinate offset(
   offset_t o) const noexcept {
    coordinate n = (*this);
    for (value_t d = 0; d < Nd; ++d) {
      // todo: addwith overflow
      signed_value_t new_xs = static_cast<signed_value_t>(xs[d]) + o[d];
      n[d]                  = static_cast<value_t>(new_xs);
      HM3_ASSERT(
       new_xs >= 0 || !n,
       "negative coordinate xs does not result in an invalid coordinate");
    }
    return n;
  }

  /// Offset all coordinate components by \p o
  ///
  /// If the coordinate is outside the tile it will be invalid.
  [[clang::no_sanitize("integer")]] constexpr coordinate offset(
   signed_value_t o) const noexcept {
    coordinate n = (*this);
    for (value_t d = 0; d < Nd; ++d) {
      // todo: addwith overflow
      signed_value_t new_xs = static_cast<signed_value_t>(xs[d]) + o;
      n[d]                  = static_cast<value_t>(new_xs);
      HM3_ASSERT(
       new_xs >= 0 || !n,
       "negative coordinate xs does not result in an invalid coordinate");
    }
    return n;
  }

  /// Offset each coordinate components by \p o if the result is valid
  ///
  /// \pre The coordinate state must be valid;
  constexpr coordinate offset_if_valid(signed_value_t o) const noexcept {
    HM3_ASSERT(*this, "the coordinate state must be valid");
    coordinate n = (*this);
    for (value_t d = 0; d < Nd; ++d) {
      auto new_n = n.offset(d, o);
      n          = new_n ? new_n : n;
    }
    return n;
  }

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream>
  friend OStream& operator<<(OStream& os, coordinate const& x) {
    os << "{" << x[0];
    for (value_t d = 1; d < Nd; ++d) { os << ", " << x[d]; }
    os << "}";
    return os;
  }

  /// \name Comparison operators
  ///
  /// Convert coordinates to indices and compare them.
  ///
  ///@{

  friend constexpr bool operator==(coordinate const& a, coordinate const& b) {
    return a.idx() == b.idx();
  }

  friend constexpr bool operator!=(coordinate const& a, coordinate const& b) {
    return !(a == b);
  }

  friend constexpr bool operator<(coordinate const& a, coordinate const& b) {
    return a.idx() < b.idx();
  }

  friend constexpr bool operator<=(coordinate const& a, coordinate const& b) {
    return a.idx() <= b.idx();
  }

  friend constexpr bool operator>(coordinate const& a, coordinate const& b) {
    return !(a <= b);
  }

  friend constexpr bool operator>=(coordinate const& a, coordinate const& b) {
    return !(a < b);
  }

  ///@}  // Comparison operators
};

/// Square distance between the coordinates \p a and \p b
///
/// \pre Both coordinates must be valid
template <suint_t Nd, suint_t Nc>
constexpr auto distance_square(coordinate<Nd, Nc> const& a,
                               coordinate<Nd, Nc> const& b) noexcept {
  HM3_ASSERT(a, "invalid coordinate {}", a);
  HM3_ASSERT(b, "invalid coordinate {}", b);
  using value_t = typename coordinate<Nd, Nc>::value_t;
  value_t dist  = 0;
  for (value_t d = 0; d < Nd; ++d) {
    dist += math::ipow(math::absdiff(b.xs[d], a.xs[d]), value_t{2});
  }
  return dist;
}

/// Distance between the coordinates \p a and \p b
///
/// \pre Both coordinates must be valid
template <suint_t Nd, suint_t Nc>
constexpr auto distance(coordinate<Nd, Nc> const& a,
                        coordinate<Nd, Nc> const& b) noexcept {
  HM3_ASSERT(a, "invalid coordinate {}", a);
  HM3_ASSERT(b, "invalid coordinate {}", b);
  return std::sqrt(static_cast<num_t>(distance_square(a, b)));
}

}  // namespace tile
}  // namespace square
}  // namespace structured
}  // namespace hm3
