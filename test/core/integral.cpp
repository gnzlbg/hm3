////////////////////////////////////////////////////////////////////////////////
/// \file Integral tests
////////////////////////////////////////////////////////////////////////////////
#include <hom3/core/test.hpp>
#include <hom3/core/integral.hpp>
////////////////////////////////////////////////////////////////////////////////

using namespace hom3;
int main() {
  // constexpr int a = integral::foldl(integral::range_c<int, 0, 5>, 0,
  // integral::sum);
  // constexpr int a = boost::hana::sum(boost::hana::range_c<int, 0, 5>);

  // static_assert(a == 10, "");

  // using list = decltype(integral::list(
  //     integral::type<integral::t<int, 0>>,
  //     integral::type<integral::t<int, 3>>,
  //     integral::type<integral::t<int, 7>>));

  // static_assert(std::is_same<integral::t<int, 0>,
  //               decltype(head(list{}))>::value, "");

  // static_assert(std::is_same<integral::t<int, 3>,
  //               decltype(head(tail(list{})))>::value, "");

  // static_assert(std::is_same<integral::t<int, 7>,
  //               decltype(head(tail(tail(list{}))))>::value, "");

  // int counter = 0;

  // integral::for_each(list{}, [&](auto i) { counter += i; });
  // CHECK(counter == 10);

  return test::result();
}
