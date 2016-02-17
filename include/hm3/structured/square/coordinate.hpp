#pragma once
/// \file
///
/// Structured grid coordinate
#include <hm3/structured/square/bounds.hpp>
#include <hm3/structured/square/index.hpp>

namespace hm3 {
namespace structured {

namespace square {

/// Coordinate of a cell within a square structured grid.
///
/// Nd: number of spatial dimensions
/// Nic: number of internal cells per dimension
/// Nhl: number of of halo layers
///
template <uint_t Nd, uint_t Nic, uint_t Nhl>  //
struct coordinate : std::array<suint_t, Nd> {
  using base = std::array<suint_t, Nd>;

  using bounds   = bounds<Nd, Nic, Nhl>;
  using idx_t    = idx<Nd, Nic, Nhl>;
  using offset_t = std::array<sint_t, Nd>;

  constexpr coordinate()                  = default;
  constexpr coordinate(coordinate const&) = default;
  constexpr coordinate(coordinate&&)      = default;

  constexpr coordinate& operator=(coordinate const&) = default;
  constexpr coordinate& operator=(coordinate&&) = default;

  /// Constructs a coordinate from the index
  constexpr coordinate(idx_t i) noexcept : coordinate(from(std::move(i))) {}

  /// True if the coordinate is valid (i.e. within the grid), false otherwise
  explicit operator bool() const noexcept {
    constexpr auto cpl = bounds::cells_per_length();
    for (suint_t d = 0; d != Nd; ++d) {
      const auto v = (*this)[d];
      if (v > cpl /*and v < 0 */) { return false; }
    }
    return true;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr coordinate from(idx_t i) noexcept {
    HM3_ASSERT(i, "invalid index");
    coordinate x;
    x[0] = *i;
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr coordinate from(idx_t i) noexcept {
    HM3_ASSERT(i, "invalid index");
    coordinate x;
    constexpr auto cpl = bounds::cells_per_length();

    const auto iv = *i;
    x[0]          = iv % cpl;  // or i - (j * cpl)
    x[1]          = iv / cpl;
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Computes the coordinates of the index \p i
  ///
  /// \pre The index must be valid (i.e. index a cell within the grid).
  /// \post The resulting coordinate will be valid.
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr coordinate from(idx_t i) noexcept {
    HM3_ASSERT(i, "invalid index");
    coordinate x;
    constexpr auto cpl = bounds::cells_per_length();

    const auto iv  = *i;
    x[0]           = iv % cpl;
    const auto tmp = iv / cpl;
    x[1]           = tmp % cpl;
    x[2]           = tmp / cpl;
    HM3_ASSERT(x, "index {} produced invalid coordinate {}", i, x);
    return x;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr idx_t index(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    idx_t i(x[0]);
    return i;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr idx_t index(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    idx_t i(x[0] + bounds::cells_per_length() * x[1]);
    return i;
  }

  /// Index corresponding to coordinate
  ///
  /// \pre The coordinate must be valid (i.e. it must index a cell in the grid)
  /// \post The resulting index will be valid.
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr idx_t index(coordinate x) noexcept {
    HM3_ASSERT(x, "invalid coordinate {}", x);
    constexpr auto cpl = bounds::cells_per_length();
    idx_t i(x[0] + cpl * x[1] + cpl * cpl * x[2]);
    return i;
  }

  /// Explicit conversion operator to index
  explicit operator idx_t() const noexcept { return index(*this); }

  /// Prints a coordinate to an output stream (for debugging)
  template <typename OStream> OStream& operator<<(OStream& os) const {
    os << "{" << (*this)[0];
    for (suint_t d = 1; d != Nd; ++d) { os << ", " << (*this)[d]; }
    os << "}";
    return os;
  }
};

}  // namespace square
}  // namespace structured
}  // namespace hm3
