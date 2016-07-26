#pragma once
/// \file
///
/// Variant
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

#include <experimental/variant.hpp>

namespace hm3 {

using std::experimental::variant;
using std::experimental::visit;

}  // namespace hm3
