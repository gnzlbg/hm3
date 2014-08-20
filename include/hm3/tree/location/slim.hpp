#pragma once
/// \file slim.hpp
#include <type_traits>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {
namespace tree {
namespace location {

template <uint_t nd, typename UInt = uint_t>  //
struct slim {
  using this_t = slim<nd, UInt>;

  using value_type     = this_t;
  using storage_type   = this_t;
  using reference_type = this_t const&;
  using integer_t      = UInt;

  static_assert(UnsignedIntegral<integer_t>{},
                "location::slim storage must be an unsigned integral type");

  integer_t value = 1;  /// Default constructed to the root node

  static constexpr uint_t dimension() noexcept { return nd; }
  static auto dimensions() noexcept { return hm3::dimensions(dimension()); }

  static constexpr uint_t no_levels() noexcept {
    constexpr auto no_bits  = bit::width<integer_t>;
    constexpr auto loc_size = nd;
    return math::floor((static_cast<double>(no_bits - loc_size))
                       / static_cast<double>(loc_size));
  }

  static constexpr level_idx max_level() noexcept { return no_levels() - 1; }

  constexpr level_idx level() const noexcept {
    HM3_ASSERT(value,
               "trying to obtain the level of an uninitialized location code");

#if defined(__GCC__) || defined(__clang__)
    // This:
    // {
    //   auto key = value;
    //   for (int d = 0; key; ++d) {
    //     if (key == 1) { return d; }
    //     key >>=3;
    //   }
    //   return key;
    // }
    // is equivalent to:
    return (bit::width<integer_t> - 1 - bit::clz(value)) / nd;
#else
#pragma message "error unsuported compiler"
#endif
  }

  void push(uint_t position_in_parent) noexcept {
    HM3_ASSERT(position_in_parent < no_children(nd),
               "position in parent {} out-of-bounds [0, {}) (nd: {})",
               position_in_parent, no_children(nd), nd);
    HM3_ASSERT(level() != max_level(),
               "location \"full\": level equals max_level {}", max_level());
    value = (value << nd) + position_in_parent;
  }

  void push(child_pos<nd> position_in_parent) { push(*position_in_parent); }
  uint_t pop() noexcept {
    HM3_ASSERT(level() > level_idx{0_u},
               "cannot pop root-node from location code");
    uint_t tmp = (*this)[level()];
    value >>= nd;
    return tmp;
  }

  bool valid() const noexcept { return value != 0; }

  uint_t operator[](const level_idx level_) const noexcept {
    HM3_ASSERT(level_ > 0_l and level_ <= level(),
               "level {} out-of-bounds [1, {})", level_, level());
    integer_t mask             = no_children(nd) - 1;
    const integer_t mask_shift = (*(level() - level_)) * nd;
    mask <<= mask_shift;
    mask &= value;
    mask >>= mask_shift;
    return mask;
  }

  auto levels() const noexcept {
    return boxed_ints<level_idx>(1_l, level() + 1_l);
  }

  auto operator()() const noexcept {
    const auto l = level();
    return (l == 0_l ? boxed_ints<level_idx>(0_l, 0_l) : levels())
           | view::transform([=](level_idx l_i) { return (*this)[*l_i]; });
  }

  slim() = default;
  slim(slim const&) = default;
  slim& operator=(slim const&) = default;
  slim(slim&&)  = default;
  slim& operator=(slim&&) = default;

  slim(std::initializer_list<uint_t> list) : slim() {
    for (auto&& p : list) { push(p); }
  }

  static constexpr std::array<integer_t, nd> set_level(
   std::array<integer_t, nd> xs, level_idx l) noexcept {
    bit::set(xs[0], *l, true);
    return xs;
  }

  static constexpr integer_t encode(std::array<integer_t, nd> xs,
                                    level_idx l) noexcept {
    return bit::morton::encode(set_level(xs, l));
  }

  static constexpr std::array<integer_t, nd> remove_level(
   std::array<integer_t, nd> xs, level_idx l) noexcept {
    bit::set(xs[0], *l, false);
    return xs;
  }

  static constexpr std::array<integer_t, nd> decode(integer_t v,
                                                    level_idx l) noexcept {
    return remove_level(bit::morton::decode(v, std::array<integer_t, nd>{}), l);
  }

  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{})>
  slim(std::array<U, nd> x_, level_idx l = (max_level() - level_idx{1})) {
    HM3_ASSERT(l < max_level(), "");

    for (auto&& d : dimensions()) {
      HM3_ASSERT(x_[d] > 0. and x_[d] < 1., "location from non-normalized "
                                            "float (d: {}, x[d]: {}) "
                                            "out-of-range (0., 1.)",
                 d, x_[d]);
    }

    num_t scale = math::ipow(2_u, *l);
    std::array<integer_t, nd> tmp;
    for (auto&& d : dimensions()) { tmp[d] = x_[d] * scale; }
    value = encode(tmp, l);

    while (level() > l) { pop(); }
  }

  // from root:
  template <class Rng, CONCEPT_REQUIRES_(Range<Rng>())>
  slim(Rng&& ps) : slim() {
    for (auto&& p : ps) { push(p); }
  }

  void reverse() {
    slim other;
    for (auto l : (*this)() | view::reverse) { other.push(l); }
    (*this) = other;
  }

  explicit operator integer_t() const noexcept { return value; }

  explicit operator std::array<integer_t, nd>() const noexcept {
    return decode(value, level());
  }

  static constexpr this_t empty_value() noexcept {
    this_t t;
    t.value = integer_t{0};
    return t;
  }
  static constexpr bool is_empty_value(this_t v) noexcept {
    return v.value == integer_t{0};
  }

  static constexpr value_type const& access_value(
   storage_type const& v) noexcept {
    return v;
  }
  static constexpr value_type const& store_value(value_type const& v) noexcept {
    return v;
  }
  static constexpr value_type&& store_value(value_type&& v) noexcept {
    return std::move(v);
  }
};

template <typename OStream, uint_t nd, typename integer_t>
OStream& operator<<(OStream& os, slim<nd, integer_t> const& lc) {
  os << "[id: " << static_cast<integer_t>(lc) << ", lvl: " << lc.level()
     << ", xs: {";
  std::array<integer_t, nd> xs(lc);
  for (auto&& d : dimensions(nd)) {
    os << xs[d];
    if (d != nd - 1) { os << ", "; }
  }
  os << "}, pip: {";
  uint_t counter = 0;
  for (auto&& pip : lc()) {
    counter++;
    os << pip;
    if (counter != *lc.level()) { os << ","; }
  }
  os << "}]";
  return os;
}

template <uint_t nd, typename integer_t>
compact_optional<slim<nd, integer_t>> shift(
 slim<nd, integer_t> t, std::array<int_t, nd> offset) noexcept {
  using sl = slim<nd, integer_t>;
  auto lvl = t.level();
  auto xs = sl::decode(t.value, static_cast<integer_t>(*lvl));
  if (none_of(dimensions(nd), [&](auto&& d) {
        return bit::overflows_on_add(xs[d], offset[d],
                                     static_cast<integer_t>(*lvl));
      })) {
    for (auto&& d : dimensions(nd)) {
      if (offset[d] > 0) {
        xs[d] += offset[d];
      } else {
        xs[d] -= static_cast<integer_t>(-offset[d]);
      }
    }
    t.value = sl::encode(xs, lvl);
    HM3_ASSERT(t.value != 0_u, "logic error, encoding delivers zero");
    return compact_optional<sl>{t};
  }
  return compact_optional<sl>{};
}

template <uint_t nd, class T>
constexpr bool operator==(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return a.value == b.value;
}

template <uint_t nd, class T>
constexpr bool operator!=(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return !(a == b);
}

template <uint_t nd, class T>
constexpr bool operator<(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return a.value < b.value;
}

template <uint_t nd, class T>
constexpr bool operator<=(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return (a == b) or (a < b);
}

template <uint_t nd, class T>
constexpr bool operator>(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return !(a.value <= b.value);
}

template <uint_t nd, class T>
constexpr bool operator>=(slim<nd, T> const& a, slim<nd, T> const& b) noexcept {
  return !(a < b);
}

static_assert(std::is_standard_layout<slim<1_u>>{}, "");
static_assert(std::is_literal_type<slim<1_u>>{}, "");
static_assert(std::is_nothrow_constructible<slim<1_u>>{}, "");
static_assert(std::is_nothrow_default_constructible<slim<1_u>>{}, "");
#ifndef __GLIBCXX__  // libstdc++-4.9 doesn't provide these
static_assert(std::is_trivially_copy_constructible<slim<1_u>>{}, "");
static_assert(std::is_trivially_move_constructible<slim<1_u>>{}, "");
static_assert(std::is_trivially_assignable<slim<1_u>, slim<1_u>>{}, "");
static_assert(std::is_trivially_copy_assignable<slim<1_u>>{}, "");
static_assert(std::is_trivially_move_assignable<slim<1_u>>{}, "");
#endif
static_assert(std::is_nothrow_copy_constructible<slim<1_u>>{}, "");
static_assert(std::is_nothrow_move_constructible<slim<1_u>>{}, "");
static_assert(std::is_nothrow_assignable<slim<1_u>, slim<1_u>>{}, "");
static_assert(std::is_nothrow_copy_assignable<slim<1_u>>{}, "");
static_assert(std::is_nothrow_move_assignable<slim<1_u>>{}, "");
static_assert(std::is_nothrow_destructible<slim<1_u>>{}, "");
static_assert(std::is_trivially_destructible<slim<1_u>>{}, "");

}  // namespace location
}  // namespace tree
}  // namespace hm3
