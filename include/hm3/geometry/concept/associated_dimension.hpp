#pragma once
/// \file
///
/// Associated dimension traits.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace trait {

template <dim_t Ad>
struct ambient_dimension {
  static constexpr dim_t ambient_dimension_ = Ad;
};

template <dim_t Ed>
struct element_dimension {
  static constexpr dim_t element_dimension_ = Ed;
};

template <dim_t Ad, dim_t Ed>
struct object : ambient_dimension<Ad>, element_dimension<Ed> {};

}  // namespace trait

namespace associated {

template <typename T, typename = void>
struct geometry_type {};

template <typename T>
struct geometry_type<T, std::void_t<typename uncvref_t<T>::geometry_type>> {
  using type = typename uncvref_t<T>::geometry_type;
};

template <typename T>
using t_ = typename geometry_type<uncvref_t<T>>::type;

template <typename T>
static constexpr auto const& v_ = t_<T>{};

}  // namespace associated

namespace trait {

template <typename Type, typename Trait>
using check_t = std::is_convertible<associated::t_<Type>, Trait>;

template <typename Type, typename Trait>
static constexpr auto check = check_t<Type, Trait>{};

// TODO: is rebind used?
template <typename T, dim_t Od>
using rebind_t = typename uncvref_t<T>::template rebind<Od>;

}  // namespace trait

}  // namespace hm3::geometry
