#pragma once
/// \file
///
/// Include std::experimental::string_view
#include <cstddef>

#if __has_include(<string_view>)

#include <experimental/string_view>

namespace hm3 {
using std::string_view;

template <typename CharT>
using basic_string_view = std::basic_string_view<CharT>;

}  // namespace hm3

#else  // otherwise, use boost::string_ref as string_view:

#include <boost/utility/string_ref.hpp>

namespace hm3 {
using string_view = boost::string_ref;

template <typename CharT>
using basic_string_view = boost::basic_string_ref<CharT>;

}  // namespace hm3
#endif

namespace hm3 {

constexpr string_view operator"" _sv(const char* str, size_t len) noexcept {
  return string_view(str, len);
}

}  // namespace hm3
