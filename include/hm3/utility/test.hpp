#pragma once
/// \file
///
/// Unit-testing utilities
///
/// Adapted from Eric Niebler's range-v3 library
/// https://github.com/ericniebler/range-v3
#include <cstdlib>
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/io/ascii.hpp>
#include <hm3/math/core.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/range.hpp>
#include <iostream>
#include <utility>

namespace hm3 {

/// Unit-testing utilities
namespace test {

namespace detail {

inline int& failures() {
  static int no_failures = 0;
  return no_failures;
}

struct approx_fn {
  template <typename T, typename U>
  static constexpr auto impl(T&&, U&&, long) -> bool {
    HM3_FATAL_ERROR("Function approx not found!");
    return false;
  }
  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, int)
   -> decltype(geometry::approx(std::forward<T>(t), std::forward<U>(u))) {
    return geometry::approx(std::forward<T>(t), std::forward<U>(u));
  }
  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   -> decltype(impl(std::forward<T>(t), std::forward<U>(u), int(0))) {
    return impl(std::forward<T>(t), std::forward<U>(u), 0);
  }
};

template <typename T>
struct ret {
 private:
  T t_;
  int lineno_;
  bool dismissed_ = false;
  char const* filename_;
  char const* expr_;

  template <typename U>
  void oops(U const& u) const {
    ascii_fmt::err("> ERROR: CHECK failed '{}'\n > \t {} ({})\n", expr_,
                   filename_, lineno_);
    if (dismissed_) {
      ascii_fmt::err("> \tEXPECTED: {}\n> \tACTUAL:   {} \n", u, t_);
    }
    ++failures();
  }
  void dismiss() noexcept { dismissed_ = true; }
  template <typename V = T>
  auto eval_(int) noexcept -> decltype(!std::declval<V>()) {
    return !t_;
  }
  bool eval_(int64_t) noexcept { return true; }

 public:
  ret(char const* filename, int lineno, char const* expr, T t)
   : t_(std::move(t)), lineno_(lineno), filename_(filename), expr_(expr) {}

  ~ret() {
    if (!dismissed_ && eval_(42)) { this->oops(42); }
  }

  template <typename U,
            CONCEPT_REQUIRES_(!std::is_floating_point<U>{}
                              && !std::is_floating_point<T>{}
                              && geometry::GeometryObject<uncvref_t<U>>{})>
  void operator==(U const& u) {
    dismiss();
    if (!approx_fn{}(t_, u)) { this->oops(u); }
  }

  template <typename U,
            CONCEPT_REQUIRES_(!std::is_floating_point<U>{}
                              && !std::is_floating_point<T>{}
                              && !geometry::GeometryObject<uncvref_t<U>>{})>
  void operator==(U const& u) {
    dismiss();
    if (!(t_ == u)) { this->oops(u); }
  }
  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{}
                                          || std::is_floating_point<T>{})>
  void operator==(U const& u) {
    dismiss();
    if (!(math::approx(t_, u))) { this->oops(u); }
  }
  template <typename U, CONCEPT_REQUIRES_(!std::is_floating_point<U>{})>
  auto operator!=(U const& u) {
    return dismiss();
    if (!(t_ != u)) { this->oops(u); }
  }
  template <typename U>
  void operator<(U const& u) {
    dismiss();
    if (!(t_ < u)) { this->oops(u); }
  }
  template <typename U>
  void operator<=(U const& u) {
    dismiss();
    if (!(t_ <= u)) { this->oops(u); }
  }
  template <typename U>
  void operator>(U const& u) {
    dismiss();
    if (!(t_ > u)) { this->oops(u); }
  }
  template <typename U>
  void operator>=(U const& u) {
    dismiss();
    if (!(t_ >= u)) { this->oops(u); }
  }
};

struct loc {
 private:
  char const* filename_;
  int lineno_;
  char const* expr_;

 public:
  loc(char const* filename, int lineno, char const* expr)
   : filename_(filename), lineno_(lineno), expr_(expr) {}

  template <typename T>
  ret<T> operator->*(T t) {
    return {filename_, lineno_, expr_, std::move(t)};
  }
};

}  // namespace detail

inline int result() {
  return detail::failures() != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

/// CHECK(ACTUAL op EXPECTED)
#define CHECK(...)                                                \
  (void)(hm3::test::detail::loc{__FILE__, __LINE__, #__VA_ARGS__} \
          ->*__VA_ARGS__) /**/

#define STATIC_CHECK(...)                                         \
  (void)(hm3::test::detail::loc{__FILE__, __LINE__, #__VA_ARGS__} \
          ->*__VA_ARGS__);                                        \
  static_assert(__VA_ARGS__, "") /**/

#define THROWS(expr, ExceptionType)                                        \
  do {                                                                     \
    bool exception_thrown_ = false;                                        \
    try {                                                                  \
      (expr);                                                              \
    } catch (const ExceptionType&) { exception_thrown_ = true; }           \
                                                                           \
    if (exception_thrown_) { break; }                                      \
                                                                           \
    HM3_FATAL_ERROR("Expression: " #expr                                   \
                    " doesn't throw an exception of type: " #ExceptionType \
                    ".");                                                  \
  } while (false)

template <typename Val, typename Rng>
void check_equal(Rng&& actual, std::initializer_list<Val> expected) {
  auto begin0 = ranges::begin(actual);
  auto end0   = ranges::end(actual);
  auto begin1 = ranges::begin(expected), end1 = ranges::end(expected);
  for (; begin0 != end0 && begin1 != end1; ++begin0, ++begin1) {
    CHECK(*begin0 == *begin1);
  }
  CHECK(begin0 == end0);
  CHECK(begin1 == end1);
}

template <typename Rng, typename Rng2>
void check_equal(Rng&& actual, Rng2&& expected) {
  auto begin0 = ranges::begin(actual);
  auto end0   = ranges::end(actual);
  auto begin1 = ranges::begin(expected);
  auto end1   = ranges::end(expected);
  for (; begin0 != end0 && begin1 != end1; ++begin0, ++begin1) {
    CHECK(*begin0 == *begin1);
  }
  CHECK(begin0 == end0);
  CHECK(begin1 == end1);
}

template <typename Rng, typename Rng2>
void check_approx_equal(Rng&& actual, Rng2&& expected) {
  auto begin0 = ranges::begin(actual);
  auto end0   = ranges::end(actual);
  auto begin1 = ranges::begin(expected);
  auto end1   = ranges::end(expected);
  for (; begin0 != end0 && begin1 != end1; ++begin0, ++begin1) {
    CHECK(std::fabs(*begin0 - *begin1) < math::eps);
  }
  CHECK(begin0 == end0);
  CHECK(begin1 == end1);
}

}  // namespace test
}  // namespace hm3
