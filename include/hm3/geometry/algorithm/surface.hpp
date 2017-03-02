#pragma once
/// \file
///
/// Access the surfaces a geometric primitive.
///
/// For a geometric primitive with element dimension Ed in ambient space Ad,
/// its surfaces have dimension Ed - 1.
///
/// For a 1D primitive they are vertices, for a 2D primitive they are edges, and
/// for a 3D primitive they are faces.
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
//#include <hm3/geometry/algorithm/face.hpp>

namespace hm3::geometry {

namespace surface_detail {

struct surface_fn {
  template <typename T, typename Index, typename UT = uncvref_t<T>,
            dim_t Ed = element_dimension_v<UT>>
  static constexpr auto surface_impl(T&& t, Index&& i) noexcept {
    switch (Ed) {
      case 1: {
        return vertex(std::forward<T>(t), std::forward<Index>(i));
      }
      case 2: {
        return edge(std::forward<T>(t), std::forward<Index>(i));
      }
      // case 3: {
      //   return face(std::forward<T>(t), std::forward<I>(i));
      // }
      default: { HM3_UNREACHABLE(); }
    }
  }

  template <typename T, typename UT = uncvref_t<T>,
            dim_t Ed = element_dimension_v<UT>>
  static constexpr auto surface_size_impl(T&& t) noexcept {
    switch (Ed) {
      case 1: {
        return vertex_size(std::forward<T>(t));
      }
      case 2: {
        return edge_size(std::forward<T>(t));
      }
      // case 3: {
      //   return face_size(std::forward<T>(t), std::forward<I>(i));
      // }
      default: { HM3_UNREACHABLE(); }
    }
  }

  template <typename T, typename Index>
  constexpr auto operator()(T&& t, Index&& i) const noexcept {
    return surface_impl(std::forward<T>(t), std::forward<Index>(i));
  }
  template <typename T>
  static constexpr auto size(T&& t) noexcept {
    return surface_size_impl(std::forward<T>(t));
  }

  template <typename T>
  static constexpr auto indices(T&& t) noexcept {
    return view::iota(dim_t{0}, dim_t{size(std::forward<T>(t))});
  }

  template <typename T, CONCEPT_REQUIRES_(!std::is_rvalue_reference<T>{})>
  constexpr auto range(T&& t) const noexcept {
    return indices(t)
           | view::transform([&t](auto&& i) { return surface_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(std::is_rvalue_reference<T>{})>
  constexpr auto range(T&& t) const noexcept {
    return indices(t)
           | view::transform([t](auto&& i) { return surface_fn{}(t, i); });
  }

  template <typename T>
  static constexpr auto first(T&& t) noexcept {
    HM3_ASSERT(size(t) > dim_t{0}, "");
    return surface_fn{}(std::forward<T>(t), 0);
  }

  template <typename T>
  static constexpr auto last(T&& t) noexcept {
    auto s = size(t);
    HM3_ASSERT(s > dim_t{0}, "");
    return surface_fn{}(std::forward<T>(t), s - 1);
  }
};

}  // namespace surface_detail

namespace {
static constexpr auto const& surface
 = static_const<surface_detail::surface_fn>::value;
}

namespace surface_detail {

struct surfaces_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(surface.range(std::forward<T>(t)));
};

}  // namespace surface_detail

namespace {
static constexpr auto const& surfaces
 = static_const<surface_detail::surfaces_fn>::value;
}

}  // namespace hm3::geometry
