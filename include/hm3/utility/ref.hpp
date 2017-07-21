#pragma once
/// \file
///
/// Reference
#include <hm3/utility/range.hpp>

namespace hm3 {

template <typename T>
struct ref_t {
  using self = ref_t<T>;
  using value_type
   = meta::if_c<std::is_rvalue_reference_v<T>, std::remove_reference_t<T>, T>;
  using reference
   = meta::if_c<std::is_rvalue_reference_v<T>, std::remove_reference_t<T>&, T>;
  using const_reference  = meta::if_c<std::is_rvalue_reference_v<T>,
                                     std::remove_reference_t<T> const&, T>;
  using rvalue_reference = meta::if_c<std::is_rvalue_reference_v<T>, T, T>;

  value_type value;

  template <typename U, CONCEPT_REQUIRES_(Same<uncvref_t<U>, uncvref_t<T>>{})>
  explicit ref_t(U&& u) : value(std::forward<U>(u)) {}

  reference get() & { return value; }
  const_reference get() const & { return value; }
  rvalue_reference get() && { return std::forward<T>(value); }
};

template <typename T>
auto ref(T&& t) -> ref_t<T&&> {
  return {std::forward<T>(t)};
}

}  // namespace hm3
