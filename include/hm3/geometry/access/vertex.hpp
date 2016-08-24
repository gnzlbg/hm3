#pragma once
/// \file
///
/// Access to vertices.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace access {

// TODO: clean this mess
//@{

template <class T, class R = void>
struct enable_if_has_vertex_index_type {
  typedef R type;
};

template <class T, class Enable = void>
struct HasVertexIndexType : std::false_type {};

template <class T>
struct HasVertexIndexType<T, typename enable_if_has_vertex_index_type<
                              typename T::vertex_index_type>::type>
 : std::true_type {};
//@}  // TODO

/// Type of the vertex index of the primitive
template <typename T>
struct vertex_index_type {
  using type = typename T::vertex_index_type;
};

template <typename T>
using vertex_index_t = typename vertex_index_type<uncvref_t<T>>::type;

template <typename Rng>
using vertex_t = uncvref_t<ranges::range_value_t<Rng>>;

namespace vertex_detail {

struct vertex_size_fn {
  template <typename T, CONCEPT_REQUIRES_(HasVertexIndexType<uncvref_t<T>>{})>
  constexpr auto operator()(T&& t) const RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   static_cast<vertex_index_t<T>>(vertex_size(std::forward<T>(t))));
};

}  // namespace vertex_detail

namespace {
static constexpr auto const& vertex_size
 = static_const<vertex_detail::vertex_size_fn>::value;
}  // namespace

namespace vertex_detail {

struct vertex_indices_fn {
  template <typename T, CONCEPT_REQUIRES_(HasVertexIndexType<uncvref_t<T>>{})>
  static constexpr auto impl(T&& t, long) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   view::iota(static_cast<vertex_index_t<T>>(0),
              static_cast<vertex_index_t<T>>(vertex_size(std::forward<T>(t)))));

  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex_indices(std::forward<T>(t)));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace vertex_detail

namespace {
static constexpr auto const& vertex_indices
 = static_const<vertex_detail::vertex_indices_fn>::value;
}  // namespace

namespace vertex_detail {

struct vertex_fn {
  template <typename T>
  constexpr auto operator()(T&& t, vertex_index_t<T> i) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(std::forward<T>(t), i));
};
}  // namespace vertex_detail

namespace {
static constexpr auto const& vertex
 = static_const<vertex_detail::vertex_fn>::value;
}  // namespace

namespace vertex_detail {

struct vertices_fn {
  template <typename T>
  struct transformer {
    T value;
    template <typename I>
    constexpr auto operator()(I&& i) const
     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(value, i));
    template <typename I>
    constexpr auto operator()(I&& i)
     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(value, i));
  };

  template <typename T, typename U,
            CONCEPT_REQUIRES_(!std::is_rvalue_reference<T>{})>
  static constexpr auto impl2(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   vertex_indices(t) | view::transform(transformer<T>{std::forward<T>(t)}));

  template <typename T, CONCEPT_REQUIRES_(std::is_rvalue_reference<T>{})>
  static constexpr auto impl2(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   vertex_indices(t)
   | view::transform(transformer<uncvref_t<T>>{std::move(t)}));

  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex_indices(t), vertex(t, 0),
                                        impl2(std::forward<T>(t)));

  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertices(std::forward<T>(t)));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace vertex_detail

namespace {
static constexpr auto const& vertices
 = static_const<vertex_detail::vertices_fn>::value;
}  // namespace

namespace vertex_detail {

struct first_vertex_fn {
  template <typename T, CONCEPT_REQUIRES_(HasVertexIndexType<uncvref_t<T>>{})>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(t, 0));
};

}  // namespace vertex_detail

namespace {
static constexpr auto const& first_vertex
 = static_const<vertex_detail::first_vertex_fn>::value;
}  // namespace

namespace vertex_detail {

struct last_vertex_fn {
  template <typename T, CONCEPT_REQUIRES_(HasVertexIndexType<uncvref_t<T>>{})>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(t, vertex_size(t) - 1));
};

}  // namespace vertex_detail

namespace {
static constexpr auto const& last_vertex
 = static_const<vertex_detail::last_vertex_fn>::value;
}  // namespace

namespace concepts {

namespace rc = ranges::concepts;

struct vertex_access {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(                  //
   rc::valid_expr(                                           //
    static_cast<vertex_index_t<UT>>(0),                      //
    static_cast<vertex_index_t<UT>>(vertex_size(t)),         //
    rc::model_of<rc::RandomAccessRange>(vertex_indices(t)),  //
    vertex(t, vertex_index_t<UT>(0)),                        //
    rc::model_of<rc::RandomAccessRange>(vertices(t))         //
    ));
};

using Vertex = vertex_access;

}  // namespace concepts

template <typename T>
using Vertex = concepts::rc::models<concepts::Vertex, uncvref_t<T>>;

}  // namespace access

using access::vertex_size;
using access::vertex;
using access::vertices;
using access::vertex_indices;
using access::first_vertex;
using access::last_vertex;

}  // namespace hm3::geometry
