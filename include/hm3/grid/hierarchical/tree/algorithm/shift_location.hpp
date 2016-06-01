#pragma once
/// \file
///
/// Shift node location by offset algorithm
#include <hm3/geometry/point.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace tree {

/// Shift node location
struct shift_location_fn {
  /// Shifts the location \p loc by normalized \p offset at \p level
  ///
  /// TODO: if offset is out of bounds -> undefined behavior (right now
  /// an assertion triggers in location's constructor)
  template <typename Loc, typename Array, dim_t Nd = Loc::dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc, Array offset) const noexcept -> Loc {
    const num_t scale = math::ipow(lidx_t{2}, *loc.level());
    offset_t<Nd> o;
    for (dim_t d = 0; d < Nd; ++d) { o[d] = offset[d] * scale; }
    return *shift(loc, o);
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
