#pragma once
////////////////////////////////////////////////////////////////////////////////
/// Includes:
#include <boost/any.hpp>
#include <utility>
#include <unordered_map>
#include <type_traits>
#include <hom3/core/types.hpp>
#include <hom3/core/terminate.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// \brief Contains input/output functionality
namespace io {

/// Property container
using properties = std::unordered_map<String, boost::any>;
/// Property type
using property = std::pair<String, boost::any>;

/// \brief Makes a property with \p name and \p value
property make_property(String name, boost::any value) {
  return property(name, value);
}

/// \brief Inserts a property with \p name and \p value into the
/// property container \p properties
///
/// \warning If a property with the same name already exists, this replaces its
/// value!
///
/// Note: std::remove_reference is used as an identity metafunction to prevent
/// template argument deduction.
template <class T>
void insert_property(properties& ps, String name,
                     std::remove_reference_t<T> value
                     = std::remove_reference_t<T>{}) {
  ps.insert(make_property(name, std::move(value)));
}

template <class T>
void insert(properties& ps, String name,
            std::remove_reference_t<T> value = std::remove_reference_t<T>{}) {
  ps.insert(make_property(name, std::move(value)));
}

/// \brief Reads property of type \p T with \p name from container \p
/// properties
///
/// \returns readed property of type \p T
template <class T> T read(const properties& ps, const String& name) {
  auto foundIt = ps.find(name);
  if (foundIt != std::end(ps)) {
    auto ret = foundIt->second;
    return boost::any_cast<T>(ret);
  } else { TERMINATE("property \"" + name + "\" not found!"); }
}

/// \brief Reads property with \p name from property container \p properties
/// into the \p value.
template <class T>
void read(const properties& ps, const String& name, T& value) {
  value = read<T>(ps, name);
}

}  // namespace io

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
