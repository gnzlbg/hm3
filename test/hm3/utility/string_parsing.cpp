#include <hm3/utility/string_parsing.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace parsing;

int main() {
  {                                                              // trim test
    test::check_equal(trim(""), view::c_str(""));                // empty string
    test::check_equal(trim(" "), view::c_str(""));               // ws only
    test::check_equal(trim("\n"), view::c_str(""));              // ws only
    test::check_equal(trim("\v"), view::c_str(""));              // ws only
    test::check_equal(trim("a b  c"), view::c_str("a b  c"));    // no ws
    test::check_equal(trim("  a b  c"), view::c_str("a b  c"));  // leading ws
    test::check_equal(trim("a b  c "), view::c_str("a b  c"));   // trailing ws
    test::check_equal(trim("  a b  c   "), view::c_str("a b  c"));
    test::check_equal(trim("  a b  c   \n"), view::c_str("a b  c"));
    test::check_equal(trim("  a b  c   \r\n"), view::c_str("a b  c"));
  }

  {                                       // remove prefix
    CHECK(!remove_prefix("foo", ""));     // empty
    CHECK(!remove_prefix("foo", ""));     // empty
    CHECK(!remove_prefix("foo", "bar"));  // empty
    test::check_equal(remove_prefix("foo", "foobar").value(),
                      view::c_str("bar"));
  }

  {  // from_chars
    CHECK(!from_chars<int>(""));

    // signed integers:
    CHECK(from_chars<char>("2").value() == make_pair(""_sv, char(2)));
    CHECK(from_chars<short>("2").value() == make_pair(""_sv, short(2)));
    CHECK(from_chars<int>("2").value() == make_pair(""_sv, 2));
    CHECK(from_chars<long>("2").value() == make_pair(""_sv, 2l));
    CHECK(from_chars<long long>("2").value() == make_pair(""_sv, 2ll));

    // unsigned integers:
    CHECK(from_chars<unsigned char>("2").value()
          == make_pair(""_sv, (unsigned char)2));
    CHECK(from_chars<unsigned short>("2").value()
          == make_pair(""_sv, (unsigned short)2));
    CHECK(from_chars<unsigned int>("2").value() == make_pair(""_sv, 2u));
    CHECK(from_chars<unsigned long>("2").value() == make_pair(""_sv, 2ul));
    CHECK(from_chars<unsigned long long>("2").value()
          == make_pair(""_sv, 2ull));

    // floating point:
    CHECK(from_chars<float>("2").value() == make_pair(""_sv, 2.f));
    CHECK(from_chars<float>("+2").value() == make_pair(""_sv, 2.f));
    CHECK(from_chars<float>("-2").value() == make_pair(""_sv, -2.f));

    CHECK(from_chars<float>("+2.01E+2").value() == make_pair(""_sv, 201.f));
    CHECK(from_chars<float>("+2.01e+2").value() == make_pair(""_sv, 201.f));

    CHECK(from_chars<float>("-0.201E-1").value() == make_pair(""_sv, -.0201f));
    CHECK(from_chars<float>("-0.201e-1").value() == make_pair(""_sv, -.0201f));

    CHECK(from_chars<double>("2").value() == make_pair(""_sv, 2.));
    CHECK(from_chars<double>("+2").value() == make_pair(""_sv, 2.));
    CHECK(from_chars<double>("-2").value() == make_pair(""_sv, -2.));

    CHECK(from_chars<double>("+2.01E+2").value() == make_pair(""_sv, 201.));
    CHECK(from_chars<double>("+2.01e+2").value() == make_pair(""_sv, 201.));

    CHECK(from_chars<double>("-0.201E-1").value() == make_pair(""_sv, -.0201));
    CHECK(from_chars<double>("-0.201e-1").value() == make_pair(""_sv, -.0201));

    CHECK(from_chars<double>("+2.01E+2 \t  -2.01e+2  +201.00").value()
          == make_pair(" \t  -2.01e+2  +201.00"_sv, 201.));
  }

  {  // parse array
    CHECK(!parse_array<array<int, 2>>(""));
    CHECK(!parse_array<array<int, 2>>("1"));
    CHECK(!parse_array<array<int, 2>>("12"));
    CHECK(parse_array<array<int, 2>>("1 2"));
    CHECK(parse_array<array<int, 2>>("1 2").value() == array<int, 2>{{1, 2}});
    CHECK(
     parse_array<array<double, 3>>("+2.01E+2 \t  -2.01e+2  +201.00").value()
     == array<double, 3>{{201., -201., 201.}});

    CHECK(!parse_named_array<array<double, 3>>(
     "array", "+2.01E+2 \t  -2.01e+2  +201.00"));

    CHECK(
     !parse_named_array<array<double, 3>>("array", "+2.01E+2 \t  -2.01e+2"));

    CHECK(!parse_named_array<array<double, 3>>(
     "array", "array +2.01E+2 \t  -2.01e+2  "));

    CHECK(parse_named_array<array<double, 3>>(
           "array", "array +2.01E+2 \t  -2.01e+2  +201.00")
           .value()
          == array<double, 3>{{201., -201., 201.}});
    CHECK(parse_named_array<array<double, 3>>(
           "array", "array+2.01E+2 \t  -2.01e+2  +201.00")
           .value()
          == array<double, 3>{{201., -201., 201.}});
  }

  return test::result();
}
