#pragma once
/// \file
///
/// Signed-distance function for a rectangle
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

// template <typename Vec>
// num_t rectangle(Vec&& x, Vec&& l) {
//   auto tmp = x.array().abs() - l().array();
//   return tmp.array().max(Eigen::Zero().array()).norm()
//          + d.array().min(Eigen::Zero().array()).maxCoeff();
// }

/// Signed-distance to hexaedron of lengths l=(lx,ly,lz)^T, centered at
/// x_c = (x,y,z)^T
///
/// TODO: implement rotation phi
template <dim_t Nd>
num_t rectangle(point<Nd> const& x, point<Nd> const& x_c,
                vec<Nd> const& l) noexcept {
  auto l_2           = .5 * l();
  num_a<Nd> distance = x_c() - l_2 - x();  // x(d) < x_c(d)
  for (dim_t d = 0; d < Nd; ++d) {
    if (x(d) > x_c(d)) {
      distance(d) = x(d) - (x_c(d) + l_2(d));  // pos if outside, neg if inside
    }
  }
  // pos if outside, neg if inside:
  const auto no_pos_values = (distance.array() > 0).count();

  if (no_pos_values == Nd) {  // corner quadrants
    return distance().norm();
  }
  if (no_pos_values < 2) {  // direct neighbors and inside
    return distance().maxCoeff();
  }
  if (Nd == 3) {                    // shorter distance is to edges
    if (distance(0) < num_t{0.}) {  // parallel x
      return std::sqrt(std::pow(distance(1), 2) + std::pow(distance(2), 2));
    }
    if (distance(1) < num_t{0.}) {  // parallel y
      return std::sqrt(std::pow(distance(0), 2) + std::pow(distance(2), 2));
    }
    if (distance(2) < num_t{0.}) {  // parallel z
      return std::sqrt(std::pow(distance(0), 2) + std::pow(distance(1), 2));
    }
  }
  HM3_FATAL_ERROR("unreachable: x: {}, x_c: {}, l: {}", x, x_c, l);
}

template <dim_t Nd>
struct fixed_rectangle {
  point<Nd> x_c;
  vec<Nd> l;

  fixed_rectangle(point<Nd> x_c_, vec<Nd> l_) noexcept : x_c(x_c_), l(l_) {}

  num_t operator()(point<Nd> const& x) const noexcept {
    return rectangle(x, x_c, l);
  }
};

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
