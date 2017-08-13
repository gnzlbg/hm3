#pragma once
/// \file
///
/// String parsing utilities
#include <hm3/ext/string_view.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/range.hpp>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <cinttypes>
namespace hm3::parsing {

/// Exception used to indicate a parsing failure.
struct failure : std::exception {
  string context;
  string expected;
  string line;
  failure(string c, string e, string l) : context(c), expected(e), line(l) {}
};

template <typename OStream>
OStream& operator<<(OStream& os, failure const& f) {
  os << "Parsing Failure while: \"" << f.context << "\"\n\nexpected: \""
     << f.expected << "\"\ncurrent line: \"" << f.line << "\"\n";
  return os;
}

constexpr string_view space_chars() noexcept {
  constexpr auto space_chars_ = " \f\n\r\t\v\0";
  return string_view(space_chars_);
}

string_view substr(string_view s, std::size_t pos,
                     std::size_t size = string_view::npos) noexcept {
  HM3_ASSERT(pos <= s.size(), "pos = {} out-of-range [0, {})", pos, s.size());
  return s.substr(pos, size);
}

/// Remove leading and trailing whitespace.
///
/// Whitespace chars = `{' ', '\f', '\n', '\r', '\t', '\v'}`.
string_view trim(string_view s) noexcept {
  auto first = s.find_first_not_of(space_chars());
  if (first == string_view::npos) { return ""; }
  auto last = s.find_last_not_of(space_chars());
  auto size = (last - first) + 1;

  return substr(s, first, size);
}

/// Removes \p prefix from \p chars.
///
/// If `chars` does not start with `prefix` returns empty.
optional<string_view> remove_prefix(string_view prefix, string_view chars) {
  HM3_ASSERT(ranges::size(prefix) > 0, "prefix of zero size, chars: {}", chars);
  // If chars does not start with prefix, returns empty
  if (ranges::size(chars) == 0
      or not ranges::equal(prefix, ranges::begin(chars))) {
    return {};
  }
  chars.remove_prefix(ranges::size(prefix));
  return chars;
}

/// Parses the first \tparam T in \p chars (ignoring leading whitespace).
///
/// If parsing success, returns a `(string_view, T)` pair, where `T` is the
/// parsed value, and the `string_view` contains the tail of the string (with
/// white-space removed).
///
/// If parsing fails, returns nothing.
template <typename T>
optional<pair<string_view, T>> from_chars(string_view chars) noexcept {
  if (chars.empty()) { return {}; }
  chars = trim(chars);
  T value;
  int ret;
  if constexpr(std::is_integral<T>()) {
      if constexpr(std::is_signed<T>()) {
          std::int64_t tmp;
          ret   = std::sscanf(chars.data(), "%" PRId64, &tmp);
          value = static_cast<T>(tmp);
      } else if constexpr(std::is_unsigned<T>()) {
          std::uint64_t tmp;
          ret   = std::sscanf(chars.data(), "%" PRIu64, &tmp);
          value = static_cast<T>(tmp);
      }
  } else if constexpr(std::is_floating_point<T>()) {
      double tmp;
      ret   = std::sscanf(chars.data(), "%lf", &tmp);
      value = static_cast<T>(tmp);
  } else {
    static_assert(always_false<T>{}, "unhandled case");
  }

  if (ret != 1) { return {}; }
  // Find the next white-space character:
  auto n = chars.find_first_of(space_chars());
  if (n == string_view::npos) {
    // If not found, advance till the end
    n = chars.size();
  }
  
  chars.remove_prefix(n);
  return make_pair(chars, value);
}

/// Parses numbers from \p chars into an \tparam Array .
template <typename Array>
optional<Array> parse_array(string_view chars) {
  using T          = ranges::range_value_type_t<Array>;
  constexpr auto N = std::tuple_size<Array>::value;
  Array a;
  for (std::size_t i = 0; i < N; ++i) {
    auto r = from_chars<T>(chars);
    if (!r) { return {}; }
    chars = first(r.value());
    a[i]  = second(r.value());
  }
  return a;
}

/// Parse "named" array: "name v0, v1, v2, ..."
template <typename Array>
optional<Array> parse_named_array(string_view name, string_view chars) {
  auto array_name = remove_prefix(name, chars);
  if (not array_name) { return {}; }
  auto array_chars = array_name.value();
  return parse_array<Array>(array_chars);
}

}  // namespace hm3::parsing
