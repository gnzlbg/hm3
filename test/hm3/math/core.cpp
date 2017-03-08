#include <hm3/math/core.hpp>
#include <hm3/math/integral/simpson_1d.hpp>
#include <hm3/math/integral/trapezoidal_1d.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using namespace hm3;
  using namespace math;

  {  // absdiff
    STATIC_CHECK(absdiff(0u, 0u) == 0u);
    STATIC_CHECK(absdiff(1u, 0u) == 1u);
    STATIC_CHECK(absdiff(0u, 1u) == 1u);
    STATIC_CHECK(absdiff(2u, 1u) == 1u);
    STATIC_CHECK(absdiff(1u, 2u) == 1u);
  }

  {  // ilog
    int32_t to = std::numeric_limits<int32_t>::max() / 10000;
    for (int32_t i = 1; i < to; ++i) {
      CHECK(ilog(2, i) == (int32_t)std::log2(i));
    }
  }

  {  // signum
    STATIC_CHECK(signum(0u) == signum_t::zero());
    STATIC_CHECK(signum(1u) == signum_t::positive());
    STATIC_CHECK(signum(2u) == signum_t::positive());
    STATIC_CHECK(signum(0) == signum_t::zero());
    STATIC_CHECK(signum(1) == signum_t::positive());
    STATIC_CHECK(signum(2) == signum_t::positive());
    STATIC_CHECK(signum(-1) == signum_t::negative());
    STATIC_CHECK(signum(-2) == signum_t::negative());
  }

  {  // ipow
    int32_t to = 20;
    for (int32_t i = 0; i < to; ++i) {
      CHECK(ipow(2, i) == (int32_t)std::pow(2, i));
      CHECK(ipow(3, i) == (int32_t)std::pow(3, i));
    }
  }

  {  // sign
    for (num_t i = -100.; i < 100.; i += 1.) {
      CHECK(fast_sign(i) == robust_sign(i));
      CHECK(sign(i) == robust_sign(i));
    }
  }

  {  // factorial
    STATIC_CHECK(factorial(0) == 1);
    STATIC_CHECK(factorial(1) == 1);
    STATIC_CHECK(factorial(2) == 2);
    STATIC_CHECK(factorial(3) == 6);
    STATIC_CHECK(factorial(4) == 24);
  }

  {  // binomial coefficient
    STATIC_CHECK(binomial_coefficient(0, 0) == 1);
    STATIC_CHECK(binomial_coefficient(1, 0) == 1);
    STATIC_CHECK(binomial_coefficient(1, 1) == 1);
    STATIC_CHECK(binomial_coefficient(2, 0) == 1);
    STATIC_CHECK(binomial_coefficient(2, 1) == 2);
    STATIC_CHECK(binomial_coefficient(2, 2) == 1);
    STATIC_CHECK(binomial_coefficient(3, 0) == 1);
    STATIC_CHECK(binomial_coefficient(3, 1) == 3);
    STATIC_CHECK(binomial_coefficient(3, 2) == 3);
    STATIC_CHECK(binomial_coefficient(3, 3) == 1);
  }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"

  {  // floor
    using ::hm3::math::floor;
    STATIC_CHECK(floor(-2.0) == -2.0);
    STATIC_CHECK(floor(-1.0) == -1.0);
    STATIC_CHECK(floor(0.0) == 0.0);
    STATIC_CHECK(floor(1.0) == 1.0);
    STATIC_CHECK(floor(2.0) == 2.0);

    STATIC_CHECK(floor(0.2) == 0.0);
    STATIC_CHECK(floor(0.5) == 0.0);
    STATIC_CHECK(floor(0.8) == 0.0);
    STATIC_CHECK(floor(1.2) == 1.0);
    STATIC_CHECK(floor(1.5) == 1.0);
    STATIC_CHECK(floor(1.8) == 1.0);

    STATIC_CHECK(floor(-0.2) == -1.0);
    STATIC_CHECK(floor(-0.5) == -1.0);
    STATIC_CHECK(floor(-0.8) == -1.0);
    STATIC_CHECK(floor(-1.2) == -2.0);
    STATIC_CHECK(floor(-1.5) == -2.0);
    STATIC_CHECK(floor(-1.8) == -2.0);
  }

  {  // ceil
    using ::hm3::math::ceil;
    STATIC_CHECK(ceil(-2.0) == -2.0);
    STATIC_CHECK(ceil(-1.0) == -1.0);
    STATIC_CHECK(ceil(0.0) == 0.0);
    STATIC_CHECK(ceil(1.0) == 1.0);
    STATIC_CHECK(ceil(2.0) == 2.0);

    STATIC_CHECK(ceil(0.2) == 1.0);
    STATIC_CHECK(ceil(0.5) == 1.0);
    STATIC_CHECK(ceil(0.8) == 1.0);
    STATIC_CHECK(ceil(1.2) == 2.0);
    STATIC_CHECK(ceil(1.5) == 2.0);
    STATIC_CHECK(ceil(1.8) == 2.0);

    STATIC_CHECK(ceil(-0.2) == -0.0);
    STATIC_CHECK(ceil(-0.5) == -0.0);
    STATIC_CHECK(ceil(-0.8) == -0.0);
    STATIC_CHECK(ceil(-1.2) == -1.0);
    STATIC_CHECK(ceil(-1.5) == -1.0);
    STATIC_CHECK(ceil(-1.8) == -1.0);
  }
#pragma clang diagnostic pop
  // Gaussian 1D: TODO
  // Gaussian 2D: TODO

  auto test_integration
   = [](std::string name, auto&& f, num_t solution, num_t from, num_t to,
        auto&& int_f, int steps = 42) {
       auto start_step = 4;
       auto offset     = 2;
       std::vector<num_t> results(steps);
       std::vector<num_t> error(steps);
       std::vector<num_t> order_next(steps);
       std::vector<num_t> order_first(steps);

       for (int s = start_step; s < steps; s += 2) {
         results[s] = int_f(f, from, to, s);
         error[s]   = std::abs(results[s] - solution);
       }

       for (int s = start_step + offset; s < steps; s += offset) {
         order_next[s] = error[s - 2] / error[s];
         order_first[s]
          = std::pow(error[start_step] / error[s], 1. / (s - start_step));
       }

       std::cout << "integrate_" << name << ":" << std::endl;
       for (int s = start_step + offset; s < steps; s += offset) {
         std::cout << "  s: " << s << " r: " << results[s] << " e: " << error[s]
                   << " on: " << order_next[s] << " of: " << order_first[s]
                   << std::endl;
       }
     };

  test_integration("trapezoidal", [](num_t v) { return std::sin(v); }, 2., 0.,
                   pi, integrate_trapezoidal);

  test_integration("simpson", [](num_t v) { return std::sin(v); }, 2., 0., pi,
                   integrate_simpson);

  return test::result();
}
