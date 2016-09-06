#pragma once
/// \file
///
/// Unit-testing utilities
///
/// Adapted from Eric Niebler's range-v3 library
/// https://github.com/ericniebler/range-v3
#include <cstdlib>
#include <hm3/types.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/math.hpp>
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

template <typename T>
struct streamable_base {};

template <typename OStream, typename T>
OStream& operator<<(OStream& sout, streamable_base<T> const&) {
  return sout << "<non-streamable type>";
}

template <typename T>
struct streamable : streamable_base<T> {
 private:
  T const& t_;

 public:
  explicit streamable(T const& t) : t_(t) {}
  template <typename OStream, typename U = T>
  friend auto operator<<(OStream& sout, streamable const& s)
   -> decltype(sout << std::declval<U const&>()) {
    return sout << s.t_;
  }
};

template <typename T>
streamable<T> stream(T const& t) {
  return streamable<T>{t};
}

struct approx_fn {
  template <typename T, typename U>
  static constexpr auto impl(T&&, U&&, num_t, long) -> bool {
    fmt::print("ERROR: approx function not found!");
    return false;
  }

  template <typename T, typename U>
  static constexpr auto impl(T&& t, U&& u, num_t tol, int)
   -> decltype(approx(std::forward<T>(t), std::forward<U>(u), tol)) {
    return approx(std::forward<T>(t), std::forward<U>(u), tol);
  }

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t tol) const
   -> decltype(impl(std::forward<T>(t), std::forward<U>(u), tol, 0)) {
    return impl(std::forward<T>(t), std::forward<U>(u), tol, 0);
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
  num_t tol_ = -1.;

  template <typename U>
  void oops(U const& u) const {
    fmt::print("> ERROR: CHECK failed '{}'\n > \t {} ({})\n", expr_, filename_,
               lineno_);
    if (dismissed_) {
      fmt::print("> \tEXPECTED: {}\n> \tACTUAL:   {} \n", stream(u),
                 stream(t_));
    }
    ++failures();
  }
  void dismiss() { dismissed_ = true; }
  template <typename V = T>
  auto eval_(int) -> decltype(!std::declval<V>()) {
    return !t_;
  }
  bool eval_(long) { return true; }

 public:
  ret(char const* filename, int lineno, char const* expr, T t)
   : t_(std::move(t)), lineno_(lineno), filename_(filename), expr_(expr) {}
  ret(num_t tol, char const* filename, int lineno, char const* expr, T t)
   : t_(std::move(t))
   , lineno_(lineno)
   , filename_(filename)
   , expr_(expr)
   , tol_{tol} {}

  ~ret() {
    if (!dismissed_ && eval_(42)) { this->oops(42); }
  }
  template <typename U, CONCEPT_REQUIRES_(!std::is_floating_point<U>{}
                                          && !std::is_floating_point<T>{})>
  void operator==(U const& u) {
    dismiss();
    if (tol_ < 0.) {
      if (!(t_ == u)) { this->oops(u); }
    } else {
      if (!approx_fn{}(t_, u, tol_)) { this->oops(u); }
    }
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
  num_t tol_ = -1.;

 public:
  loc(char const* filename, int lineno, char const* expr)
   : filename_(filename), lineno_(lineno), expr_(expr) {}

  loc(num_t tol, char const* filename, int lineno, char const* expr)
   : filename_(filename), lineno_(lineno), expr_(expr), tol_(tol) {}

  template <typename T>
  ret<T> operator->*(T t) {
    return {tol_, filename_, lineno_, expr_, std::move(t)};
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

#define TOL_CHECK(tol, ...)                                            \
  (void)(hm3::test::detail::loc{tol, __FILE__, __LINE__, #__VA_ARGS__} \
          ->*__VA_ARGS__) /**/

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
