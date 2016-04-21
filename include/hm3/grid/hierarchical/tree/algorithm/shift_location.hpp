#pragma once
/// \file
///
/// Shift node location by offset algorithm
#include <array>
#include <hm3/geometry/point.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace tree {

/// Shift node location
struct shift_location_fn {
  /// Shifts the location \p loc by normalized \p offset at \p level
  ///
  /// TODO: if offset is out of bounds -> undefined behavior (right now
  /// an assertion triggers in location's constructor)
  template <typename Loc, dim_t Nd = Loc::dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc, std::array<num_t, Nd> offset,
                  lidx_t level = Loc::max_level()) const noexcept -> Loc {
    const num_t scale = math::ipow(lidx_t{2}, loc.level);
    RANGES_FOR (auto&& d, dimensions(Nd)) { offset[d] += loc[d] / scale; }
    return Loc{offset, level};
  }

  /// Shifts the location \p loc by \p offset
  ///
  /// If the resulting location is out-of-bounds the optional_location won't
  /// contain a valid value.
  template <typename Loc, dim_t Nd = Loc::dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc, offset_t<Nd> offset) const noexcept
   -> compact_optional<Loc> {
    return shift(loc, offset);
  }
};

namespace {
constexpr auto&& shift_location = static_const<shift_location_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
