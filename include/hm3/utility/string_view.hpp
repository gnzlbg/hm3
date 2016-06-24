#pragma once
/// \file
///
///  std::experimental::string_view
#include <cstddef>
#if defined(_cpp_lib_experimental_string_view) \
 && _cpp_lib_experimental_string_view >= 201411
#include <experimental/string_view>

namespace hm3 {
using std::string_view;
}  // namespace hm3
#else

#include <hm3/utility/boost/string_ref.hpp>

namespace hm3 {
using string_view = boost::string_ref;
}  // namespace hm3
#endif
