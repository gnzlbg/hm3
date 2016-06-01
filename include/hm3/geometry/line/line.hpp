#pragma once
/// \file
///
/// Line
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd>  //
struct line : dimensional<Nd> {
  point<Nd> x_0, x_1;
  /// Line that goes through the 2 points \p b and \p e
  static line<Nd> through(point<Nd> b, point<Nd> e) noexcept {
    line<Nd> tmp;
    tmp.x_0 = b;
    tmp.x_1 = e;
    return tmp;
  }
  auto operator()() const noexcept {
    return Eigen::ParametrizedLine<num_t, Nd>::Through(x_0(), x_1());
  }

  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>{})>
  constexpr line(Rng&& rng) noexcept {
    HM3_ASSERT(ranges::distance(rng) == 2, "?");
    auto b = begin(rng);
    x_0    = *b;
    ++b;
    x_1 = *b;
  }

  constexpr line()            = default;
  constexpr line(line const&) = default;
  constexpr line(line&&)      = default;
  constexpr line& operator=(line const&) = default;
  constexpr line& operator=(line&&) = default;
};

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, line<Nd> const& l) {
  o << "(x_0{";
  for (dim_t d = 0; d < Nd; ++d) {
    o << l.x_0(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << "}, x_1{";
  for (dim_t d = 0; d < Nd; ++d) {
    o << l.x_1(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << "})";
  return o;
}

}  // namespace geometry
}  // namespace hm3
