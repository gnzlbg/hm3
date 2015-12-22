#pragma once
/// \file
///
/// Slim location implementation
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <type_traits>

namespace hm3 {
namespace tree {
namespace location {

template <uint_t Nd, typename UInt = uint_t>  //
struct slim : geometry::dimensional<Nd> {
  using self = slim<Nd, UInt>;

  using value_type     = self;
  using storage_type   = self;
  using reference_type = self const&;
  using integer_t      = UInt;

  static_assert(UnsignedIntegral<integer_t>{},
                "location::slim storage must be an unsigned integral type");

  integer_t value = 1;  /// Default constructed to the root node

  using geometry::dimensional<Nd>::dimension;
  using geometry::dimensional<Nd>::dimensions;

  static constexpr uint_t no_levels() noexcept {
    constexpr auto no_bits  = bit::width<integer_t>;
    constexpr auto loc_size = Nd;
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
    return (bit::width<integer_t> - 1 - bit::clz(value)) / Nd;
#else
#pragma message "error unsuported compiler"
#endif
  }

  void push(uint_t position_in_parent) noexcept {
    HM3_ASSERT(position_in_parent < no_children(Nd),
               "position in parent {} out-of-bounds [0, {}) (Nd: {})",
               position_in_parent, no_children(Nd), Nd);
    HM3_ASSERT(level() != max_level(),
               "location \"full\": level equals max_level {}", max_level());
    value = (value << Nd) + position_in_parent;
  }

  void push(child_pos<Nd> position_in_parent) { push(*position_in_parent); }
  uint_t pop() noexcept {
    HM3_ASSERT(level() > level_idx{0_u},
               "cannot pop root-node from location code");
    uint_t tmp = (*this)[level()];
    value >>= Nd;
    return tmp;
  }

  bool valid() const noexcept { return value != 0; }

  uint_t operator[](const level_idx level_) const noexcept {
    HM3_ASSERT(level_ > 0_l and level_ <= level(),
               "level {} out-of-bounds [1, {})", level_, level());
    integer_t mask             = no_children(Nd) - 1;
    const integer_t mask_shift = (*(level() - level_)) * Nd;
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

  slim()            = default;
  slim(slim const&) = default;
  slim& operator=(slim const&) = default;
  slim(slim&&)                 = default;
  slim& operator=(slim&&) = default;

  slim(std::initializer_list<uint_t> list) : slim() {
    for (auto&& p : list) { push(p); }
  }

  static constexpr std::array<integer_t, Nd> set_level(
   std::array<integer_t, Nd> xs, level_idx l) noexcept {
    bit::set(xs[0], *l, true);
    return xs;
  }

  static constexpr integer_t encode(std::array<integer_t, Nd> xs,
                                    level_idx l) noexcept {
    return bit::morton::encode(set_level(xs, l));
  }

  static constexpr std::array<integer_t, Nd> remove_level(
   std::array<integer_t, Nd> xs, level_idx l) noexcept {
    bit::set(xs[0], *l, false);
    return xs;
  }

  static constexpr std::array<integer_t, Nd> decode(integer_t v,
                                                    level_idx l) noexcept {
    return remove_level(bit::morton::decode(v, std::array<integer_t, Nd>{}), l);
  }

  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{})>
  slim(std::array<U, Nd> x_, level_idx l = (max_level() - level_idx{1})) {
    HM3_ASSERT(l < max_level(), "");

    for (auto&& d : dimensions()) {
      HM3_ASSERT(x_[d] > 0. and x_[d] < 1., "location from non-normalized "
                                            "float (d: {}, x[d]: {}) "
                                            "out-of-range (0., 1.)",
                 d, x_[d]);
    }

    num_t scale = math::ipow(2_su, *l);
    std::array<integer_t, Nd> tmp;
    for (auto&& d : dimensions()) { tmp[d] = x_[d] * scale; }
    value = encode(tmp, l);

    while (level() > l) { pop(); }
  }

  // from root:
  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>())>
  slim(Rng&& ps) : slim() {
    for (auto&& p : ps) { push(p); }
  }

  void reverse() {
    slim other;
    for (auto l : (*this)() | view::reverse) { other.push(l); }
    (*this) = other;
  }

  explicit operator integer_t() const noexcept { return value; }

  explicit operator std::array<integer_t, Nd>() const noexcept {
    return decode(value, level());
  }

  static constexpr self empty_value() noexcept {
    self t;
    t.value = integer_t{0};
    return t;
  }
  static constexpr bool is_empty_value(self v) noexcept {
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

template <typename OStream, uint_t Nd, typename Int>
OStream& operator<<(OStream& os, slim<Nd, Int> const& lc) {
  os << "[id: " << static_cast<Int>(lc) << ", lvl: " << lc.level() << ", xs: {";
  std::array<Int, Nd> xs(lc);
  for (auto&& d : dimensions(Nd)) {
    os << xs[d];
    if (d != Nd - 1) { os << ", "; }
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

template <uint_t Nd, typename Int>
compact_optional<slim<Nd, Int>> shift(slim<Nd, Int> t,
                                      std::array<int_t, Nd> offset) noexcept {
  using sl = slim<Nd, Int>;
  auto lvl = t.level();
  auto xs  = sl::decode(t.value, static_cast<Int>(*lvl));
  if (none_of(dimensions(Nd), [&](auto&& d) {
        return bit::overflows_on_add(xs[d], offset[d], static_cast<Int>(*lvl));
      })) {
    for (auto&& d : dimensions(Nd)) {
      if (offset[d] > 0) {
        xs[d] += offset[d];
      } else {
        xs[d] -= static_cast<Int>(-offset[d]);
      }
    }
    t.value = sl::encode(xs, lvl);
    HM3_ASSERT(t.value != 0_u, "logic error, encoding delivers zero");
    return compact_optional<sl>{t};
  }
  return compact_optional<sl>{};
}

template <uint_t Nd, typename T>
constexpr bool operator==(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
  return a.value == b.value;
}

template <uint_t Nd, typename T>
constexpr bool operator!=(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
  return !(a == b);
}

template <uint_t Nd, typename T>
constexpr bool operator<(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
  return a.value < b.value;
}

template <uint_t Nd, typename T>
constexpr bool operator<=(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
  return (a == b) or (a < b);
}

template <uint_t Nd, typename T>
constexpr bool operator>(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
  return !(a.value <= b.value);
}

template <uint_t Nd, typename T>
constexpr bool operator>=(slim<Nd, T> const& a, slim<Nd, T> const& b) noexcept {
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
