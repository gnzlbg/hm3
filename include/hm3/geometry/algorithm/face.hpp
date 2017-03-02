#pragma once
/// \file
///
/// Access to the faces of a geometric primitive.
#include <hm3/geometry/concept/face_access.hpp>

namespace hm3::geometry {

namespace face_detail {

struct face_fn {
  template <typename T, typename IndexType, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  constexpr auto operator()(T &&t, IndexType &&i) const noexcept {
    return face(std::forward<T>(t), std::forward<IndexType>(i));
  }
  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  static constexpr auto size(T &&t) noexcept {
    return face_size(std::forward<T>(t));
  }

  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  static constexpr auto indices(T &&t) noexcept {
    return view::iota(associated::face_index_t<T>(0),
                      associated::face_index_t<T>(size(std::forward<T>(t))));
  }

  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{} and
                                          !std::is_rvalue_reference<T>{})>
  constexpr auto range(T &&t) const noexcept {
    return indices(t) |
           view::transform([&t](auto &&i) { return face_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{} and
                                          std::is_rvalue_reference<T>{})>
  constexpr auto range(T &&t) const noexcept {
    return indices(t) |
           view::transform([t](auto &&i) { return face_fn{}(t, i); });
  }

  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  static constexpr auto first(T &&t) noexcept {
    HM3_ASSERT(size(t) > associated::face_index_t<T>(0), "");
    return face_fn{}(std::forward<T>(t), 0);
  }

  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  static constexpr auto last(T &&t) noexcept {
    auto s = size(t);
    HM3_ASSERT(s > associated::face_index_t<T>(0), "");
    return face_fn{}(std::forward<T>(t), s - 1);
  }
};

} // namespace face_detail

namespace {
static constexpr auto const &face = static_const<face_detail::face_fn>::value;
}

namespace face_detail {

struct faces_fn {
  template <typename T, CONCEPT_REQUIRES_(FaceAccess<T>{})>
  constexpr auto operator()(T &&t) const
    RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(face.range(std::forward<T>(t)));
};

} // namespace face_detail

namespace {
static constexpr auto const &faces = static_const<face_detail::faces_fn>::value;
}

} // namespace hm3::geometry
