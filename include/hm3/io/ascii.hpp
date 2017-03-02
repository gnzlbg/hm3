#pragma once
/// \file
///
/// ASCII OStream writer.
#include <hm3/geometry/io/ascii.hpp>
#include <hm3/grid/hierarchical/tree/serialization/ascii.hpp>
#include <hm3/utility/ascii.hpp>

namespace hm3::io::ascii {

namespace op {

template <typename OStream, typename T>
constexpr auto operator<<(OStream& os, T&& t)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(to_ascii(os, std::forward<T>(t)));

}  // namespace op

}  // namespace hm3::io::ascii
