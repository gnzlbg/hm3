// #pragma once
// /// \file
// ///
// /// Projects one geometric primitive onto another
// #include <hm3/geometry/algorithm/project/line_to_cartesian_plane.hpp>

// namespace hm3::geometry {

// namespace project_detail {
// struct project_fn {
//   template <typename T, typename U,
//             CONCEPT_REQUIRES_(Line<T>{} and CartesianPlane<U>{})>
//   static constexpr auto impl(T const& t, U const& u) noexcept {
//     return project_line_to_cartesian_plane(t, u);
//   }

//   template <typename T, typename U>
//   constexpr operator()(T&& t, U&& u) const
//    RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
//                                              std::forward<U>(u)));
// };
// }  // namespace project_detail

// namespace {
// static constexpr auto const& project
//  = static_const<project_detail::project_fn>::value;
// }

// }  // namespace hm3::geometry
