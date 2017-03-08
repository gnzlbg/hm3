#pragma once
/// \file
///
/// ASCII OStream writer.
#include <hm3/ext/variant.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/optional.hpp>
#include <iomanip>

namespace hm3::ascii_fmt {

namespace utility_detail {

template <typename OStream, typename Rng, CONCEPT_REQUIRES_(Range<Rng>{})>
constexpr OStream& range_to_ascii_impl(OStream& os, Rng&& rng) {
  os << "[";
  auto len = ranges::size(rng);
  for (std::size_t i = 0; i != len; ++i) {
    to_ascii(os, ranges::at(rng, i));
    if (i != len - 1) { os << ","; }
  }
  os << "]";
  return os;
}

template <typename OStream, typename T>
constexpr OStream& to_ascii_impl(OStream& o, optional<T> const& t) {
  if (t) {
    to_ascii(o, t.value());
  } else {
    o << "-";
  }
  return o;
}

template <typename OStream, typename T, typename U>
constexpr OStream& to_ascii_impl(OStream& os, compact_optional<T, U> const& a) {
  if (a) {
    to_ascii(os, *a);
  } else {
    os << '_';
  }
  return os;
};

template <typename OStream, typename T, typename U>
constexpr OStream& to_ascii_impl(OStream& os,
                                 ranges::compressed_pair<T, U> const& t) {
  os << "(";
  to_ascii(os, first(t));
  os << ", ";
  to_ascii(os, second(t));
  os << ")";
  return os;
}

template <typename OStream, typename T, typename U>
constexpr OStream& to_ascii_impl(OStream& os, std::pair<T, U> const& t) {
  os << "(";
  to_ascii(os, first(t));
  os << ", ";
  to_ascii(os, second(t));
  os << ")";
  return os;
}

template <typename OStream>
constexpr OStream& to_ascii_impl(OStream& os, monostate const&) {
  os << "-";
  return os;
}

template <typename OStream, typename T>
constexpr OStream& to_ascii_impl(OStream& os, T* const& t) {
  if (t == nullptr) {
    os << "-";
  } else {
    os << "ptr: ";
    to_ascii(os, *t);
  }
  return os;
}

template <typename OStream, typename... Args>
constexpr OStream& to_ascii_impl(OStream& os, variant<Args...> const& v) {
  os << "| ";
  visit([&os](auto&& i) { to_ascii(os, i); }, v);
  os << " |";
  return os;
}

}  // namespace utility_detail

template <typename OStream, typename T>
constexpr auto to_ascii(OStream& os, T const& t, utility const&)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(utility_detail::to_ascii_impl(os, t));

template <typename OStream, typename T>
constexpr auto to_ascii(OStream& os, T const& t, range_like const&)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(utility_detail::range_to_ascii_impl(os,
                                                                          t));

}  // namespace hm3::ascii_fmt
