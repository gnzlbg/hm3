#pragma once
/// \file
///
/// Polygon
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/line.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/utility/stack_vector.hpp>

namespace hm3 {
namespace geometry {

template <uint_t nd>
struct dynamic_polygon : dimensional<nd>, std::vector<point<nd>> {
  using self = std::vector<point<nd>>;
  using self::self;
  using self::operator=;
};

template <uint_t nd, uint_t MaxNp>
struct polygon : dimensional<nd>, stack::vector<point<nd>, MaxNp> {
  using self = stack::vector<point<nd>, MaxNp>;
  using self::self;
  using self::operator=;
  auto corner_positions() const noexcept {
    return view::iota(std::size_t{0}, this->size());
  }
};

template <typename T> struct is_polygon : std::false_type {};
template <uint_t Nd> struct is_polygon<dynamic_polygon<Nd>> : std::true_type {};

template <uint_t Nd, uint_t MaxNp>
struct is_polygon<polygon<Nd, MaxNp>> : std::true_type {};

template <uint_t Nd, uint_t MaxNp>
bool operator==(polygon<Nd, MaxNp> const& a,
                polygon<Nd, MaxNp> const& b) noexcept {
  return equal(a, b);
}

template <uint_t Nd, uint_t MaxNp>
bool operator!=(polygon<Nd, MaxNp> const& a,
                polygon<Nd, MaxNp> const& b) noexcept {
  return !(a == b);
}

/// Square corners
template <uint_t Nd, uint_t MaxNp>
constexpr auto corners(polygon<Nd, MaxNp> p) noexcept {
  return p;
}

template <typename OStream, uint_t Nd, uint_t MaxNp>
OStream& operator<<(OStream& os, polygon<Nd, MaxNp> pol) {
  os << "[";
  uint_t c = 0;

  for (auto&& p : pol) {
    os << "x_" << c << "{ ";
    for (auto d : dimensions(Nd)) {
      os << p(d);
      if (d != Nd - 1) { os << ", "; }
    }
    os << "}";
    if (c != pol.size() - 1) { os << ", "; }
    ++c;
  }

  os << "]";
  return os;
}

template <typename Polygon, typename P = std::decay_t<Polygon>,
          CONCEPT_REQUIRES_(is_polygon<P>{} and P::dimension() == 2)>
constexpr bool is_sorted_counter_clock_wise(Polygon&& ps) {
  const auto e = ps.size() - 1;
  num_t tmp    = 0.;
  for (suint_t i = 0; i != e; ++i) {
    tmp += (ps[i + 1](0) - ps[i](0)) * (ps[i + 1](1) + ps[i](1));
  }
  {  // wrap around
    tmp += (ps[0](0) - ps[e](0)) * (ps[0](1) + ps[e](1));
  }
  return tmp < 0;
}

template <typename Polygon,
          CONCEPT_REQUIRES_(is_polygon<std::decay_t<Polygon>>{})>
constexpr bool is_sorted_clock_wise(Polygon&& ps) {
  return !is_sorted_counter_clock_wise(ps);
}

/// Compute the signed area of a two dimensional polygon
template <typename Polygon, typename P = std::decay_t<Polygon>,
          CONCEPT_REQUIRES_(is_polygon<P>{} and P::dimension() == 2)>
constexpr num_t signed_area(Polygon&& polygon) noexcept {
  num_t a         = 0.;
  const suint_t e = size(polygon) - 1;
  const auto& ps  = polygon;
  for (suint_t i = 1; i != e; ++i) {
    a += ps[i](0) * (ps[i + 1](1) - ps[i - 1](1));
  }
  {
    a += ps[e](0) * (ps[0](1) - ps[e - 1](1));
    a += ps[0](0) * (ps[1](1) - ps[0](1));
  }
  return a / 2.;
}

template <typename Polygon,
          CONCEPT_REQUIRES_(is_polygon<std::decay_t<Polygon>>{})>
constexpr num_t area(Polygon&& polygon) noexcept {
  return std::abs(signed_area(std::forward<Polygon>(polygon)));
}

template <typename Polygon, typename P = std::decay_t<Polygon>,
          CONCEPT_REQUIRES_(is_polygon<P>{} and P::dimension() == 2)>
constexpr point<P::dimension()> centroid(Polygon&& polygon) noexcept {
  HM3_ASSERT(is_sorted_counter_clock_wise(polygon),
             "polygon not sorted counter clock wise!");
  point<P::dimension()> x = point<P::dimension()>::constant(0.);

  auto a = signed_area(polygon);

  const auto e = size(polygon) - 1;
  auto&& ps    = polygon;
  auto kernel  = [&](auto&& l, auto&& r) {
    return (ps[l](0) * ps[r](1) - ps[r](0) * ps[l](1)) * (ps[l]() + ps[r]());
  };
  for (suint_t i = 0; i != e; ++i) { x() += kernel(i, i + 1); }
  {  // wrap around
    x() += kernel(e, 0);
  }

  x() /= (6. * a);
  return x;
}

}  // namespace geometry
}  // namespace hm3
