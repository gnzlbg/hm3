////////////////////////////////////////////////////////////////////////////////
/// \file Tests the Arithmetic class
#include <hom3/core/arithmetic.hpp>
#include <hom3/core/test.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

namespace hom3 {
/// Instantiate everything:
template struct arithmetic<int, void>;
}  // namespace hom3

void conversions() {
  /// Should be constructible from its underlying type
  auto a = arithmetic<Ind>{2};

  /// Should be explicitly convertible to its underlying type
  Ind b = a();
  Ind c = 2;
  CHECK(b == c);
  auto lambda = [](Ind i) { return i; };
  CHECK(lambda(a()) == c);

  /// Should not be implicitly convertible to its underlying type
  // Ind d = a; // does not compile :)
  // lambda(a); // does not compile :)

  /// Should not implicitly convert to integer types of different families
  struct class1 {};
  struct class2 {};
  auto e = arithmetic<Ind, class1>{2};
  auto f = arithmetic<Ind, class2>{3};
  // e = f; // does not compile :)
  e = f();
}

void primitive_cast() { struct class1 { };
  using T1 = arithmetic<Ind, class1>;
  struct class2 {};
  using T2 = arithmetic<Ind, class2>;

  auto a = T1{2};
  auto b = T2{3};

  primitive_cast(a) = primitive_cast(b);
  CHECK(a == T1{3});
}

template <class T> void test_compound_assignment() {
  auto i1 = T{1};
  auto i2 = T{2};
  i1 += i1;
  CHECK(i1 == i2);
  i2 -= i2;
  CHECK(i2 == T{0});
  i1 *= T{2};
  CHECK(i1 == T{4});
  i1 /= T{2};
  CHECK(i1 == T{2});
}

void compound_assignment() {
  test_compound_assignment<arithmetic<Ind>>();
  test_compound_assignment<arithmetic<Int>>();
}

template <class T> void test_unsigned_arithmetic() {
  auto i1 = T{1};
  auto i2 = i1 + i1 + i1;
  auto i3 = i1 * T{3};
  CHECK(i2 == i3);
  CHECK((i3 / i2) == T{1});
  auto i4 = i3 - T{2} * i1;
  CHECK(i4 == i1);
}

void arithmetic_operators() {
  test_unsigned_arithmetic<arithmetic<Ind>>();
  test_unsigned_arithmetic<arithmetic<Int>>();

  // signed integers:
  using I = arithmetic<Int>;
  auto i1 = I{1};
  auto i3 = i1 - I{2} * i1;
  CHECK(i3 == -i1);
}

template <class T> void test_unsigned_increment_operators() {
  auto i1 = T{1};
  auto i2 = T{2};
  ++i1;
  CHECK(i1 == i2);
  auto i3 = i2;
  CHECK(i3 == i2++);
  CHECK(i2 == T{3});
  --i2;
  CHECK(i2 == i1);
  auto i4 = i1;
  CHECK(i4 == i1--);
}

void increment_operators() {
  test_unsigned_increment_operators<arithmetic<Ind>>();
  test_unsigned_increment_operators<arithmetic<Int>>();
}

template <class T> void test_comparison_operators() {
  auto a = T{2};
  auto b = T{3};
  auto c = T{5};
  auto d = T{1};
  CHECK((a + b) == c);
  CHECK(a < b);
  CHECK(a <= b);
  CHECK(b <= b);
  CHECK(b >= b);
  CHECK(b > a);
  CHECK(b >= a);
  CHECK(b == b);
  CHECK(b != a);
  CHECK((b - a) == d);
  CHECK((a * b) == c + d);

  auto a_old = a;
  a += b;
  CHECK(a == c);
  b -= a_old;
  CHECK(b == d);
}

void comparison_operators() {
  test_comparison_operators<arithmetic<Ind>>();
  test_comparison_operators<arithmetic<Int>>();
}

int main() {
  conversions();
  primitive_cast();
  compound_assignment();
  arithmetic_operators();
  increment_operators();
  comparison_operators();
  return test::result();
}
