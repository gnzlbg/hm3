/// \file
///
/// Test the dimension concept and utilities
#include <hm3/geometry/dimension.hpp>
#include <hm3/utility/test.hpp>

struct A {
  static constexpr auto dimension() noexcept {
    return std::integral_constant<hm3::dim_t, 2>{};
  }
  static auto dimensions() noexcept {
    return hm3::view::iota(hm3::dim_t{0}, hm3::dim_t{2});
  }
};

struct A2 {
  static constexpr auto dimension() noexcept {
    return std::integral_constant<hm3::dim_t, 1>{};
  }
};

namespace other {

struct B {};

constexpr auto dimension(B const&) {
  return std::integral_constant<hm3::dim_t, 3>{};
}

inline auto dimensions(B const&) {
  return hm3::view::iota(hm3::dim_t{0}, hm3::dim_t{3});
}

}  // namespace other

namespace other {

struct B2 {};

constexpr auto dimension(B2 const&) {
  return std::integral_constant<hm3::dim_t, 3>{};
}

}  // namespace other

template <hm3::dim_t Nd, typename T>  //
constexpr void check_dimensions(T&&) {
  STATIC_CHECK(hm3::geometry::dimension(T{}) == Nd);
  {
    using namespace hm3;
    STATIC_CHECK(dimension(T{}) == Nd);
    hm3::dim_t c = 0;
    RANGES_FOR (auto&& d, dimensions(T{})) {
      c++;
      CHECK(d < Nd);
    }
    CHECK(c == Nd);
  }
  {
    using namespace hm3::geometry;
    STATIC_CHECK(dimension(T{}) == Nd);
    hm3::dim_t c = 0;
    RANGES_FOR (auto&& d, dimensions(T{})) {
      c++;
      CHECK(d < Nd);
    }
    CHECK(c == Nd);
  }

  static_assert(hm3::geometry::Dimensional<T>{}, "");
  static_assert(hm3::geometry::Dimensional<T, Nd>{}, "");
}

int main() {
  check_dimensions<2>(A{});
  check_dimensions<1>(A2{});
  check_dimensions<3>(other::B{});
  check_dimensions<3>(other::B2{});

  check_dimensions<1>(hm3::geometry::dimensional<1>{});
  check_dimensions<2>(hm3::geometry::dimensional<2>{});
  check_dimensions<3>(hm3::geometry::dimensional<3>{});

  int a[3];

  static_assert(!hm3::geometry::Dimensional<decltype(a)>{}, "");
  static_assert(!hm3::geometry::Dimensional<decltype(a), 2>{}, "");

  return hm3::test::result();
}
