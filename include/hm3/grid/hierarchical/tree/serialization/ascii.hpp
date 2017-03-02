#pragma once
/// \file
///
/// ASCII OStream writer.
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
#include <hm3/utility/ascii.hpp>

namespace hm3::ascii_fmt {

namespace tree_detail {

using namespace ::hm3::tree;

/// Serializes the location \p lc into an OStream \p os (for debugging)
template <typename OStream, dim_t Ad, typename Int>
OStream& to_ascii_impl(OStream& os, location::interleaved<Ad, Int> const& lc) {
  os << "[id: " << static_cast<Int>(lc) << ", lvl: ";
  to_ascii(os, lc.level());
  os << ", xs: {";
  using morton_x_t = typename location::interleaved<Ad, Int>::morton_x_t;
  morton_x_t xs(lc);
  for (auto&& d : ambient_dimension[lc]) {
    os << xs[d];
    if (d != Ad - 1) { os << ", "; }
  }
  os << "}, pip: {";
  lidx_t level_counter = 0;
  for (auto&& pip : lc()) {
    level_counter++;
    os << pip;
    if (level_counter != *lc.level()) { os << ","; }
  }
  os << "}]";
  return os;
}

}  // namespace tree_detail

template <typename OStream, typename T>
auto to_ascii(OStream& os, T const& t, tree const&)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(tree_detail::to_ascii_impl(os, t));

}  // namespace hm3::ascii_fmt
