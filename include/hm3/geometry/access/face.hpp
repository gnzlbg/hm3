#pragma once
/// \file
///
/// Access to faces.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace access {

/// Type of the face index of the primitive
template <typename T>
struct face_index_type {
  using type = typename T::face_index_type;
};

template <typename T>
using face_index_t = typename face_index_type<uncvref_t<T>>::type;

template <typename Rng>
using face_t = uncvref_t<ranges::range_value_t<Rng>>;

namespace face_detail {

struct face_size_fn {
  template <typename T, typename IT = face_index_t<T>>
  constexpr auto operator()(T&& t) const RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   static_cast<IT>(face_size(std::forward<T>(t))));
};

}  // namespace face_detail

namespace {
static constexpr auto const& face_size
 = static_const<face_detail::face_size_fn>::value;
}  // namespace

namespace face_detail {

struct face_indices_fn {
  template <typename T, typename IT = face_index_t<T>>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(view::iota(
    static_cast<IT>(0), static_cast<IT>(face_size(std::forward<T>(t)))));

  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face_indices(std::forward<T>(t)));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace face_detail

namespace {
static constexpr auto const& face_indices
 = static_const<face_detail::face_indices_fn>::value;
}  // namespace

namespace face_detail {

struct face_fn {
  template <typename T>
  constexpr auto operator()(T&& t, face_index_t<T> i) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face(std::forward<T>(t), i));
};
}  // namespace face_detail

namespace {
static constexpr auto const& face = static_const<face_detail::face_fn>::value;
}  // namespace

namespace face_detail {

struct faces_fn {
  template <typename T>
  struct transformer {
    T value;
    template <typename I>
    constexpr auto operator()(I&& i) const
     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face(value, i));
    template <typename I>
    constexpr auto operator()(I&& i)
     RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face(value, i));
  };

  template <typename T, typename U,
            CONCEPT_REQUIRES_(!std::is_rvalue_reference<T>{})>
  static constexpr auto impl2(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   face_indices(t) | view::transform(transformer<T>{std::forward<T>(t)}));

  template <typename T, CONCEPT_REQUIRES_(std::is_rvalue_reference<T>{})>
  static constexpr auto impl2(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   face_indices(t) | view::transform(transformer<uncvref_t<T>>{std::move(t)}));

  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face_indices(t), face(t, 0),
                                        impl2(std::forward<T>(t)));

  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(faces(std::forward<T>(t)));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace face_detail

namespace {
static constexpr auto const& faces = static_const<face_detail::faces_fn>::value;
}  // namespace

namespace concepts {

namespace rc = ranges::concepts;

struct face_access {
  template <typename T, typename UT = uncvref_t<T>>
  static auto requires_(T&& t) -> decltype(                //
   rc::valid_expr(                                         //
    static_cast<face_index_t<UT>>(0),                      //
    static_cast<face_index_t<UT>>(face_size(t)),           //
    rc::model_of<rc::RandomAccessRange>(face_indices(t)),  //
    face(t, face_index_t<UT>(0)),                          //
    rc::model_of<rc::RandomAccessRange>(faces(t))          //
    ));
};

using Face = face_access;

}  // namespace concepts

template <typename T>
using Face = concepts::rc::models<concepts::Face, uncvref_t<T>>;

}  // namespace access

using access::face_size;
using access::face;
using access::faces;
using access::face_indices;

}  // namespace hm3::geometry
