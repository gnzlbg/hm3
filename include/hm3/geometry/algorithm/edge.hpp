#pragma once
/// \file
///
/// Access to the edges of a geometric primitive.
#include <hm3/geometry/concept/edge_access.hpp>

namespace hm3::geometry {

namespace edge_detail {

struct edge_fn {
  template <typename T, typename IndexType, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  constexpr auto operator()(T&& t, IndexType&& i) const noexcept {
    return edge(std::forward<T>(t), std::forward<IndexType>(i));
  }
  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  static constexpr auto size(T&& t) noexcept {
    return edge_size(std::forward<T>(t));
  }

  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  static constexpr auto indices(T&& t) noexcept {
    return view::iota(associated::edge_index_t<T>(0),
                      associated::edge_index_t<T>(size(std::forward<T>(t))));
  }

  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{}
                                          and !std::is_rvalue_reference<T>{})>
  constexpr auto range(T&& t) const noexcept {
    return indices(t)
           | view::transform([&t](auto&& i) { return edge_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{}
                                          and std::is_rvalue_reference<T>{})>
  constexpr auto range(T&& t) const noexcept {
    return indices(t)
           | view::transform([t](auto&& i) { return edge_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  static constexpr auto first(T&& t) noexcept {
    HM3_ASSERT(size(t) > associated::edge_index_t<T>(0), "");
    return edge_fn{}(std::forward<T>(t), 0);
  }

  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  static constexpr auto last(T&& t) noexcept {
    auto s = size(t);
    HM3_ASSERT(s > associated::edge_index_t<T>(0), "");
    return edge_fn{}(std::forward<T>(t), s - 1);
  }
};

}  // namespace edge_detail

namespace {
static constexpr auto const& edge = static_const<edge_detail::edge_fn>::value;
}

namespace edge_detail {

struct edges_fn {
  template <typename T, CONCEPT_REQUIRES_(EdgeAccess<T>{})>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(edge.range(std::forward<T>(t)));
};

}  // namespace edge_detail

namespace {
static constexpr auto const& edges = static_const<edge_detail::edges_fn>::value;
}

}  // namespace hm3::geometry
