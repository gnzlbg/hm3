#pragma once
/// \file properties.hpp I/O properties
#include <utility>
#include <unordered_map>
#include <type_traits>
#include <hm3/types.hpp>
#include <hm3/utility/any.hpp>
#include <hm3/utility/terminate.hpp>

namespace hm3 {

/// I/O functionality
namespace io {

namespace detail {
using any_type = hm3::any;

template <class T, class U> auto any_type_cast(U&& value) -> T {
  return hm3::any_cast<T>(std::forward<U>(value));
}

}  // namespace detail

/// Property container
using properties = std::unordered_map<string, detail::any_type>;
/// Property type
using property = std::pair<string, detail::any_type>;

/// Inserts a property with \p name and \p value into the property container \p
/// properties
template <class T>
void insert(properties& ps, string const& name,
            std::remove_reference_t<T> value) {
  detail::any_type o(value);
  auto p = std::make_pair(name, std::move(o));
  ps.insert(p);
}

/// Reads property of type \p T with \p name from container \p properties
///
/// \returns the property value
template <class T> T& read(properties& ps, string const& name) {
  auto foundIt = ps.find(name);
  if (foundIt != std::end(ps)) {
    auto& ret = foundIt->second;
    return detail::any_type_cast<T&>(ret);
  } else {
    HM3_TERMINATE("property \"{}\" not found!", name);
  }
}

template <class T> T const& read(properties const& ps, string const& name) {
  auto foundIt = ps.find(name);
  if (foundIt != std::end(ps)) {
    auto const& ret = foundIt->second;
    return detail::any_type_cast<T const&>(ret);
  } else {
    HM3_TERMINATE("property \"{}\" not found!", name);
  }
}

/// Reads property with \p name from property container \p properties into the
/// \p value.
template <class T>
void read(properties const& ps, string const& name, T& value) {
  value = read<T>(ps, name);
}

}  // namespace io
}  // namespace hm3
