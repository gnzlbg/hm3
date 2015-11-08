#pragma once
/// \file
///
/// I/O properties
/*
#include <utility>
#include <unordered_map>
#include <type_traits>
#include <hm3/types.hpp>
#include <hm3/utility/any.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

/// I/O functionality
namespace io {

namespace detail {
using any_type = hm3::any;

template <typename T, typename U> auto any_type_cast(U&& value) -> T {
  return hm3::any_cast<T>(std::forward<U>(value));
}

}  // namespace detail

/// Property container
using properties = std::unordered_map<string, detail::any_type>;
/// Property type
using property = std::pair<string, detail::any_type>;

/// Inserts a property with \p name and \p value into the property container \p
/// properties
template <typename T>
void insert(properties& ps, string const& name,
            std::remove_reference_t<T> value) {
  detail::any_type o(value);
  auto p = std::make_pair(name, std::move(o));
  ps.insert(p);
}

/// Reads property of type \p T with \p name from container \p properties
///
/// \returns the property value
template <typename T> T& read(properties& ps, string const& name) {
  auto found_it = ps.find(name);
  if (found_it != ranges::end(ps)) {
    auto& ret = found_it->second;
    return detail::any_type_cast<T&>(ret);
  }
  HM3_FATAL_ERROR("property \"{}\" not found!", name);
}

template <typename T> T const& read(properties const& ps, string const& name) {
  auto found_it = ps.find(name);
  if (found_it != ranges::end(ps)) {
    auto const& ret = found_it->second;
    return detail::any_type_cast<T const&>(ret);
  }
  HM3_FATAL_ERROR("property \"{}\" not found!", name);
}

/// Reads property with \p name from property container \p properties into the
/// \p value.
template <typename T>
void read(properties const& ps, string const& name, T& value) {
  value = read<T>(ps, name);
}

}  // namespace io
}  // namespace hm3
*/
