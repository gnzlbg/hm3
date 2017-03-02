#pragma once
/// \file
///
/// Signed-distance function for an axis-aligned bounding box.
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/geometry/sd/transformations/origin_rotation.hpp>
#include <hm3/geometry/sd/transformations/translation.hpp>

namespace hm3::geometry::sd {

namespace aabb_detail {
struct aabb_fn {
  /// Signed-distance to the aabb of lengths l=(LC,ly,lz)^T, centered at x_c =
  /// (x,y,z)^T
  template <typename P, typename V>
  constexpr num_t operator()(P const& x, V const& l) const noexcept {
    auto d = (x.array().abs() - 0.5 * l.array()).eval();
    return d.array().max(V::Zero().array()).matrix().norm()
           + d.array().min(V::Zero().array()).maxCoeff();
  }
};
}  // namespace aabb_detail

namespace {
static constexpr auto const& aabb = static_const<aabb_detail::aabb_fn>::value;
}

template <dim_t Ad>
struct aabb_at_origin : with_ambient_dimension<Ad> {
  vec<Ad> lengths_ = vec<Ad>::constant(1.);

  constexpr aabb_at_origin() = default;
  template <typename V>
  constexpr aabb_at_origin(V lengths) noexcept
   : lengths_(std::move(lengths())) {
    static_assert(Vector<V>{});
    HM3_ASSERT(
     [&]() {
       for (dim_t d = 0; d < Ad; ++d) {
         HM3_ASSERT(lengths_(d) > 0., "aabb length {} = {} !> 0.", d,
                    lengths_(d));
       }
       return true;
     }(),
     "");
  }

  void set_length(num_t length, dim_t d) noexcept {
    HM3_ASSERT(length > 0., "cannot set aabb length {} to {} !> 0.", d, length);
    lengths_(d) = length;
  }

  template <typename V>
  void set_lengths(V lengths) noexcept {
    HM3_ASSERT(
     [&]() {
       for (dim_t d = 0; d < Ad; ++d) {
         HM3_ASSERT(lengths(d) > 0., "cannot set aabb length {} to {} !> 0.", d,
                    lengths(d));
       }
       return true;
     }(),
     "");
    lengths_ = std::move(lengths());
  }

  template <typename P>
  constexpr num_t operator()(P const& x) const noexcept {
    return invoke_unwrapped(aabb, x, lengths_);
  }
};

template <dim_t Ad>
using aabb_at_point = translation<aabb_at_origin<Ad>>;

template <dim_t Ad>
using rotated_aabb_at_point = translation<origin_rotation<aabb_at_origin<Ad>>>;

}  // namespace hm3::geometry::sd
