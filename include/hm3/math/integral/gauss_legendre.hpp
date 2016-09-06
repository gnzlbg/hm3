#pragma once
/// \file
///
/// Gauss-Legendre quadrature points.

namespace hm3::math {

namespace math_detail {

/// \name Gauss-Legendre quadrature points in the interval [-1, +1.]
///@{

/// Location in the interval [-1, +1.] and weights of the Gauss-Legendre
/// quadrature points.
///
/// \tparam Nd number of spatial dimensions of the interval.
/// \tparam Nppd number of integration points per spatial dimension.
///
/// For dimensions > 1 the quadrature points are constructed using tensor
/// products.
template <dim_t Nd, dim_t Nppd>
struct gauss_legendre_points {};

template <dim_t Nd>
struct gauss_legendre_points<Nd, 1> {
  using p_t = geometry::point<Nd>;
  static constexpr array<p_t, 1> points() noexcept {
    constexpr array<p_t, 1> ps = {p_t::zero()};
    return ps;
  }
  static constexpr array<num_t, 1> weights() noexcept {
    constexpr array<num_t, 1> ws = {2.};
    return ws;
  }

}

template <>
struct gauss_legendre_points<1, 2> {
  using p_t = geometry::point<1>;
  static constexpr array<p_t, 2> points() noexcept {
    constexpr num_t v = 1. / std::sqrt(1. / 3.);
    constexpr array<p_t, 2> ps = {
     p_t{-v},  // p0
     p_t{+v}   // p1
    };
    return ps;
  }
  static constexpr array<num_t, 2> weights() noexcept {
    constexpr array<num_t, 2> ws = {
     1.,  // w0
     1.   // w1
    };
    return ws;
  }
};

template <>
struct gauss_legendre_points<2, 2> {
  using p_t = geometry::point<2>;
  static constexpr array<p_t, 4> points() noexcept {
    constexpr num_t v = 1. / std::sqrt(1. / 3.);
    constexpr array<p_t, 4> ps = {
     p_t{-v, -v},  // p0
     p_t{+v, -v},  // p1
     p_t{+v, +v},  // p2
     p_t{-v, +v},  // p3
    };
    return ps;
  }
  static constexpr array<num_t, 4> weights() noexcept {
    constexpr array<num_t, 4> ws = {
     1.,  // w0
     1.,  // w1
     1.,  // w2
     1.   // w3
    };
    return ws;
  }
};

template <>
struct gauss_legendre_points<3, 2> {
  using p_t = geometry::point<3>;
  static constexpr array<p_t, 8> points() noexcept {
    constexpr num_t v = 1. / std::sqrt(1. / 3.);
    constexpr array<p_t, 8> ps = {
     p_t{-v, -v, -v},  // p0
     p_t{+v, -v, -v},  // p1
     p_t{+v, +v, -v},  // p2
     p_t{-v, +v, -v},  // p3
     p_t{-v, -v, +v},  // p4
     p_t{+v, -v, +v},  // p5
     p_t{+v, +v, +v},  // p6
     p_t{-v, +v, +v},  // p7

    };
    return ps;
  }
  static constexpr array<num_t, 8> weights() noexcept {
    constexpr array<num_t, 8> ws = {
     1.,  // w0
     1.,  // w1
     1.,  // w2
     1.,  // w3
     1.,  // w4
     1.,  // w5
     1.,  // w6
     1.   // w7
    };
    return ws;
  }
};

///@}  //  Gauss-Legendre quadrature points in the interval [-1, +1.]

template <dim_t Nd, dim_t Np>
array<geometry::point<Nd>, Np> to_interval(
 box<Nd> b, array<geometry::point<Nd>, Np> unit_points) {
  auto a = geometry::x_min(b);
  auto b = geometry::x_max(b);

  auto b_m_a_h = (b() - a()) * 0.5;
  auto a_p_b_h = (a() + b()) * 0.5;

  for (auto& p : unit_points) { p = p_t{a_m_b_h * p() + a_p_b_h}; }
  return unit_points;
}

}  // namespace math_detail

template <typename F, dim_t Nd, dim_t Nppd = 2>
num_t integral_gauss_legendre(F&& f, box<Nd> b) {
  using qp               = gauss_legendre_points<Nd, Nppd>;
  constexpr auto weights = qp::weights();
  auto points            = to_interval(b, qp::points());

  num_t result = 0.;
  for (suint_t pidx = 0, e = points.size(); pidx != e; ++pidx) {
    result += weights[pidx] * f(points[pidx]);
  }

  result *= geometry::volume(b) / math::ipow(2, Nd);
  return result;
}

}  // namespace hm3::math
