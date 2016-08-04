#pragma once
/// \file
///
/// Access to vertices.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace access {

/// Type of the vertex index of the primitive
template <typename T>  //
struct vertex_index_type {
  using type = typename T::vertex_index_type;
};

template <typename T>
using vertex_index_t = typename vertex_index_type<uncvref_t<T>>::type;

template <typename Rng> using vertex_t = uncvref_t<ranges::range_value_t<Rng>>;

namespace detail {

struct vertex_size_fn {
  template <typename T, typename IT = vertex_index_t<T>>
  constexpr auto operator()(T&& t) const RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   static_cast<IT>(vertex_size(std::forward<T>(t))));
};

}  // namespace detail

namespace {
static constexpr auto const& vertex_size
 = static_const<detail::vertex_size_fn>::value;
}  // namespace

namespace detail {

struct vertex_indices_fn {
  template <typename T, typename IT = vertex_index_t<T>>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(view::iota(
    static_cast<IT>(0), static_cast<IT>(vertex_size(std::forward<T>(t)))));

  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex_indices(std::forward<T>(t)));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace detail

namespace {
static constexpr auto const& vertex_indices
 = static_const<detail::vertex_indices_fn>::value;
}  // namespace

namespace detail {

struct vertex_fn {
  template <typename T>
  constexpr auto operator()(T&& t, vertex_index_t<T> i) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(vertex(std::forward<T>(t), i));
};
}  // namespace detail

namespace {
static constexpr auto const& vertex = static_const<detail::vertex_fn>::value;
}  // namespace

namespace detail {

struct vertices_fn {
  template <typename T>  //
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

}  // namespace detail

namespace {
static constexpr auto const& vertices
 = static_const<detail::vertices_fn>::value;
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

}  // namespace discrete

using discrete::vertex_size;
using discrete::vertex;
using discrete::vertices;
using discrete::vertex_indices;

}  // namespace geometry
}  // namespace hm3