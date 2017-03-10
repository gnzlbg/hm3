#pragma once
/// \file
///
/// MPI tag.
#include <hm3/ext/string_view.hpp>
#include <hm3/types.hpp>

namespace hm3::mpi {

/// MPI tag type.
///
/// The tag is obtained by hashing a string.
struct tag {
  tag(const char* tag_name)  // TODO: use string_view, make constexpr
   : tag_(std::hash<string>{}(tag_name)), name_(tag_name) {}
  int tag_;
  const char* name_;

  int value() const noexcept { return tag_; }
  string_view name() const noexcept { return name_; }

  constexpr bool operator==(tag const& b) const noexcept {
    return value() == b.value();
  }
  constexpr bool operator!=(tag const& b) const noexcept {
    return value() != b.value();
  }
  constexpr bool operator<(tag const& b) const noexcept {
    return value() < b.value();
  }
  constexpr bool operator<=(tag const& b) const noexcept {
    return value() <= b.value();
  }
  constexpr bool operator>(tag const& b) const noexcept {
    return value() > b.value();
  }
  constexpr bool operator>=(tag const& b) const noexcept {
    return value() >= b.value();
  }
};

namespace literals {

inline tag operator""_t(const char* n, std::size_t) { return tag{n}; }

}  // namespace literals

}  // namespace hm3::mpi
