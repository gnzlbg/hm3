#pragma once
/// \file
///
/// ASCII OStream writer.
#include <hm3/grid/hierarchical/tree/location/deinterleaved.hpp>
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
#include <hm3/utility/ascii.hpp>

namespace hm3::ascii_fmt {

namespace tree_detail {

using namespace ::hm3::tree;

/// Serializes the location \p lc into an OStream \p os (for debugging)
template <typename OStream, typename Loc, CONCEPT_REQUIRES_(Location<Loc>{})>
OStream& to_ascii_impl(OStream& os, Loc const& lc) {
  static constexpr dim_t ad = ad_v<Loc>;
  using int_t               = morton_idx_t<Loc>;
  using x_t                 = typename Loc::morton_x_t;
  os << "[id: " << static_cast<int_t>(lc) << ", lvl: ";
  to_ascii(os, lc.level());
  os << ", xs: {";
  x_t xs(lc);
  for (auto&& d : ambient_dimension[lc]) {
    os << xs[d];
    if (d != ad - 1) { os << ", "; }
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
