#pragma once
/// \file
///
/// Access to vertices of an any geometry.
#include <hm3/geometry/access/vertex.hpp>
#include <hm3/geometry/primitive/any/any.hpp>

namespace hm3::geometry::any_primitive {

/// Number of vertices in a any.
template <dim_t Nd>
constexpr dim_t vertex_size(any<Nd> const& a) noexcept {
  return visit(a, [](auto&& i) { return vertex_size(i); });
}

/// Vertex \p v of the any geometry \p a.
template <dim_t Nd>
constexpr point<Nd> vertex(any<Nd> const& a, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(a), "any geometry vertex {} out-of-bounds [0,{})",
             v, vertex_size(a));
  return visit(a, [&v](auto&& i) { return vertex(i, v); });
}

/// Vertices of the any geometry \p a.
template <dim_t Nd>
constexpr auto vertices(any<Nd> const& a) noexcept {
  return view::iota(0, vertex_size(a))
         | view::transform([&a](dim_t vidx) { return vertex(a, vidx); });
}

/// Vertices of the any geometry \p a.
template <dim_t Nd>
constexpr auto vertices(any<Nd>&& a) noexcept {
  return view::iota(0, vertex_size(a))
         | view::transform([a_ = std::move(a)](dim_t vidx) {
             return vertex(a_, vidx);
           });
}

}  // namespace hm3::geometry::any_primitive
