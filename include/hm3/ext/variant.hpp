#pragma once
/// \file
///
/// Variant
#include <hm3/ext/range-v3.hpp>
#include <type_traits>
#include <utility>

// workaround missing "is_trivially_copyable" in libstdc++ < 5.0
// TODO: wrong the version of libstdc++ is probably too high
#if defined(__GLIBCXX__) && __GLIBCXX__ < 20150421
namespace std {
template <typename T>
struct is_trivially_copyable : integral_constant<bool, __has_trivial_copy(T)> {
};
}  // namespace std
#endif  // GLIBCXX macro

#include <mpark/variant.hpp>

namespace hm3 {

using mpark::variant;
using mpark::monostate;
using mpark::get_if;

namespace variant_detail {

struct visit_fn {
  template <typename... Args>
  constexpr auto operator()(Args&&... args) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    mpark::visit(std::forward<Args>(args)...));
};

}  // namespace variant_detail

namespace {
constexpr auto const& visit
 = ::ranges::static_const<variant_detail::visit_fn>::value;
}  // namespace

}  // namespace hm3

#define HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T) \
  static_assert(always_false<T>{}, "non-exhaustive visitor")
