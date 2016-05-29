#pragma once
/// \file
///
/// Visualization field
#include <functional>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>
#include <string>

namespace hm3 {
namespace vis {

/// Visualization field
struct field {
  enum value_t : int { inactive = 0, active = 1 };

  field(string n, std::function<void(void)> f, int v = 0)
   : name{std::move(n)}, value{static_cast<value_t>(v)}, load{std::move(f)} {}
  field(string n, std::function<void(void)> f, value_t v)
   : name{std::move(n)}, value{v}, load{std::move(f)} {}

  string name;
  value_t value;
  std::function<void(void)> load;
};

inline bool operator==(field const& a, field const& b) noexcept {
  return a.name == b.name && a.value == b.value;
}

inline bool operator!=(field const& a, field const& b) noexcept {
  return !(a == b);
}

struct field_hash {
  inline std::size_t operator()(field f) const noexcept {
    return std::hash<string>{}(f.name);
  }
};

struct equal_field_type {
  inline bool operator()(field const& a, field const& b) const noexcept {
    return a.name == b.name;
  }
};

inline bool operator==(field const& a, const char* name) noexcept {
  return a.name == name;
}

/// Has the container \p c a field \p name
template <typename C, typename UC = ranges::uncvref_t<C>,
          CONCEPT_REQUIRES_(Container<UC>{}  //&&
                            // Same<value_type<UC>, field>{}
                            )>
bool has_field(C&& c, string const& name) noexcept {
  const auto count
   = distance(c | view::filter([&](auto&& i) { return i.name == name; }));
  HM3_ASSERT(count == 0 || count == 1, "either one or none");
  return count == 1 ? true : false;
}

template <typename C, typename UC = ranges::uncvref_t<C>,
          CONCEPT_REQUIRES_(Container<UC>{}  //&&
                            // Same<value_type<UC>, field>{}
                            )>
auto find_field(C&& c, string const& name) noexcept {
  return find_if(c, [&](auto&& i) { return i.name == name; });
}

template <typename C, typename UC = ranges::uncvref_t<C>,
          CONCEPT_REQUIRES_(Container<UC>{}  //&&
                            // Same<value_type<UC>, field>{}
                            )>
decltype(auto) get_field(C&& c, string const& name) noexcept {
  HM3_ASSERT(has_field(c, name), "fieldhas to exist! check with has_field!");
  return *find_field(c, name);
}

}  // namespace vis
}  // namespace hm3
