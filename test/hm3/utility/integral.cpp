/// \file integral.cpp Integral tests
#include <hm3/utility/test.hpp>
#include <hm3/utility/meta.hpp>

using namespace hm3;

int main() {
  {
    constexpr auto a
     = meta::fold<meta::as_list<meta::make_integer_sequence<std::size_t, 5>>,
                  meta::size_t<0>, meta::quote<meta::plus>>{};

    static_assert(a == 10, "");
  }

  {
    constexpr auto a
     = meta::fold<meta::as_list<meta::integer_range<std::size_t, 0, 5>>,
                  meta::size_t<0>, meta::quote<meta::plus>>{};

    static_assert(a == 10, "");
  }

  {
    constexpr auto a
     = meta::fold<meta::as_list<meta::integer_range<std::size_t, 5, 10>>,
                  meta::size_t<0>, meta::quote<meta::plus>>{};

    static_assert(a == 35, "");
  }

  using list = meta::list<meta::size_t<0>, meta::size_t<3>, meta::size_t<7>>;

  int counter = 0;

  meta::for_each(list{}, [&](auto i) { counter += i; });
  CHECK(counter == 10);

  return test::result();
}
