#pragma once
/// \file
///
/// Access to the vertices of a geometric primitive.
#include <hm3/geometry/concept/vertex_access.hpp>

namespace hm3::geometry {

namespace vertex_detail {

struct vertex_fn {
  template <typename T, typename IndexType,
            CONCEPT_REQUIRES_(VertexAccess<T>{})>
  constexpr auto operator()(T&& t, IndexType&& i) const noexcept {
    return vertex(std::forward<T>(t), std::forward<IndexType>(i));
  }
  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  static constexpr auto size(T&& t) noexcept {
    return vertex_size(std::forward<T>(t));
  }

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  static constexpr auto indices(T&& t) noexcept {
    return view::iota(associated::vertex_index_t<T>(0),
                      associated::vertex_index_t<T>(size(std::forward<T>(t))));
  }

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{}
                                          and !std::is_rvalue_reference<T>{})>
  constexpr auto range_impl(T&& t, fallback) const noexcept {
    return indices(t)
           | view::transform([&t](auto&& i) { return vertex_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{}
                                          and std::is_rvalue_reference<T>{})>
  constexpr auto range_impl(T&& t, fallback) const noexcept {
    return indices(t)
           | view::transform([t](auto&& i) { return vertex_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  constexpr auto range_impl(T&& t, preferred) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertices(std::forward<T>(t)));

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  constexpr auto range(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(range_impl(std::forward<T>(t),
                                                   dispatch));

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  static constexpr auto first(T&& t) noexcept {
    HM3_ASSERT(size(t) > associated::vertex_index_t<T>(0), "");
    return vertex_fn{}(std::forward<T>(t), 0);
  }

  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  static constexpr auto last(T&& t) noexcept {
    auto s = size(t);
    HM3_ASSERT(s > associated::vertex_index_t<T>(0), "");
    return vertex_fn{}(std::forward<T>(t), s - 1);
  }
};

}  // namespace vertex_detail

namespace {
constexpr auto const& vertex = static_const<vertex_detail::vertex_fn>::value;
}

namespace vertex_detail {

struct vertices_fn {
  template <typename T, CONCEPT_REQUIRES_(VertexAccess<T>{})>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex.range(std::forward<T>(t)));
};

}  // namespace vertex_detail

namespace {
constexpr auto const& vertices
 = static_const<vertex_detail::vertices_fn>::value;
}

}  // namespace hm3::geometry
