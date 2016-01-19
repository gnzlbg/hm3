#pragma once
/// \file
///
/// Variant
#include <type_traits>
#include <utility>

// workaround missing "is_trivially_copyable" in libstdc++ < 5.0
#if defined(__GLIBCPP__) &&__GLIBCPP__ < 20150801  // TODO: find version of libstdc++ < 5.0
namespace std {
template <typename T>
struct is_trivially_copyable : integral_constant<bool, __has_trivial_copy(T)> {
};
}  // namespace std
#endif

#include <variant.hpp>

namespace hm3 {

using std::experimental::variant;
using std::experimental::visit;

}  // namespace hm3
