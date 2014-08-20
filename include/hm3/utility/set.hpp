#pragma once
/// \file set.hpp Set utilities
#include <hm3/utility/assert.hpp>

namespace hm3 {
namespace set {

/// Replace a \p key on the set \p set with the
/// TotalOrderPreservingMutation \p m.
///
/// \tparam Set A std::set
/// \tparam Key The key type of `Set`
/// \tparam WeakTotalOrderPreservingMutation A mutation from the old key in the
///         set to a new key that preserves the order, that is:
///         !(old < new) && (new < old)
/// \par Complexity
/// \f$ O(logN) \f$ where \f$ N \f$ is the number of elements in the set.
/// \pre The \p key must be a member of the \p set.
template <class Set, class Key, class WeakTotalOrderPreservingMutation>
auto replace(Set&& set, Key&& key,
             WeakTotalOrderPreservingMutation&& m) noexcept -> void {
  auto i = set.find(std::forward<Key>(key));
  HM3_ASSERT(i != std::end(set), "key is not in the set");
  auto new_key = m(*i);
  HM3_ASSERT(!(new_key < *i) && !(i < new_key),
             "mutator is not an order preserving mutation");
  set.erase(i);
  set.insert(new_key);
}

}  // namespace set
}  // namespace hm3
