#pragma once
/// \file
///
/// Shift node location by offset algorithm
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/math/core.hpp>
#include <hm3/utility/array.hpp>

namespace hm3::tree {

/// Shift node location
struct shift_location_fn {
  /// Shifts the location \p loc by normalized \p offset at \p level
  ///
  /// If offset is out of bounds returns an empty location.
  template <typename Loc, typename Array, dim_t Ad = Loc::ambient_dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc, Array offset) const noexcept
   -> compact_optional<Loc> {
    const num_t scale = math::ipow(uint_t{2}, uint_t{*loc.level()});
    HM3_ASSERT(scale > 0., "scale !> 0!");
    offset_t<Ad> o;

    for (dim_t d = 0; d < Ad; ++d) { o[d] = offset[d] * scale; }

    return shift(loc, o);
  }

  /// Shifts the location \p loc by \p offset
  ///
  /// If the resulting location is out-of-bounds the optional_location won't
  /// contain a valid value.
  template <typename Loc, dim_t Ad = Loc::ambient_dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc, offset_t<Ad> offset) const noexcept
   -> compact_optional<Loc> {
    return shift(loc, offset);
  }
};

namespace {
constexpr auto const& shift_location = static_const<shift_location_fn>::value;
}  // namespace

}  // namespace hm3::tree
