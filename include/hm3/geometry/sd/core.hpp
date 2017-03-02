#pragma once
/// \file
///
/// Signed-distance core.
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/signed_distance.hpp>
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/geometry/sd/views/view.hpp>

namespace hm3::geometry {

/// Implicit signed-distance functions.
///
/// Outside:  g(x) > 0 outside,
/// Surface: g(x) = 0, and
/// Inside:  g(x) < 0 .
namespace sd {

// All the signed-distance functions are implemented for Eigen types, these
// utilities allows passing hm3 point/vector types with Eigen types
// transparently down the stack.

template <typename T>
auto unwrap_impl(T&& t, int) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t());

template <typename T>
auto unwrap_impl(T&& t, long)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(std::forward<T>(t));

/// Unwraps type t: if its an Eigen type it returns t, otherwise if its a
/// Point/Vector it returns the Eigen3Map.
template <typename T>
auto unwrap(T&& t)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(unwrap_impl(std::forward<T>(t), 0));

/// Invokes callable \p f with unwrapped arguments args
template <typename F, typename... Args>
auto invoke_unwrapped(F&& f, Args&&... args)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(f(unwrap(std::forward<Args>(args))...));

}  // namespace sd

}  // namespace hm3::geometry
