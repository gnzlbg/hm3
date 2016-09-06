#pragma once
/// \file
///
/// Variant
#include <hm3/utility/range.hpp>
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

#include <experimental/monostate.hpp>
#include <experimental/variant.hpp>

namespace hm3 {

using std::experimental::variant;
using std::experimental::monostate;

namespace variant_detail {

struct visit_fn {
  template <typename... Args>
  constexpr auto operator()(Args&&... args) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    std::experimental::visit(std::forward<Args>(args)...));
};

}  // namespace variant_detail

namespace {
static constexpr auto const& visit
 = static_const<variant_detail::visit_fn>::value;
}

/// Instantiating this type fails
template <typename T>
struct fail;

}  // namespace hm3

namespace std::experimental {
template <typename OStream>
OStream& operator<<(OStream& os, monostate const&) {
  os << "-";
  return os;
}

template <typename OStream, typename T>
auto print_val(OStream& os, T&& t, int) -> decltype(os << t) {
  return os << t;
}

template <typename OStream, typename T>
auto print_val(OStream& os, T&& t, long)
 -> decltype(os << ranges::view::all(t)) {
  return os << ranges::view::all(t);
}

template <typename OStream, typename... Args>
OStream& operator<<(OStream& os, variant<Args...> const& v) {
  ::hm3::visit([&os](auto&& i) { print_val(os, i, 0); }, v);
  return os;
}

}  // namespace std::experimental
