#pragma once
/// \file
///
/// This should be a location that is very fast to offset but it turns out
/// it isn't
///
/// The only use of this is for testing purposes (making sure that the tree code
/// implementation is independent of the location implementation)
///
/// \todo Remove this
#include <array>
#include <hm3/tree/types.hpp>
#include <hm3/tree/concepts.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {
namespace tree {

namespace location {

/// Location within a tree of dimension Nd
///
/// The underlying storage type T determines the maximum level
/// within the tree that can be represented.
///
/// TODO: reconsider the storage format
/// TODO: make level a member function
/// TODO: make x and level private
/// TODO: rework optional location
/// TODO: check overflow from array of floats
///
/// Right now optimized for insertion but makes it a pain for shifting
/// coordinates and doing useful stuff with the location hashes
///
template <uint_t Nd, typename T = uint_t> struct fast {
  using this_t         = fast<Nd, T>;
  using opt_this_t     = compact_optional<this_t>;
  using value_type     = this_t;
  using storage_type   = this_t;
  using reference_type = this_t const&;
  using integer_t      = T;

  std::array<T, Nd> x;
  level_idx level_ = 0;

  constexpr level_idx level() const noexcept { return level_; }

  static constexpr auto dimension() noexcept { return Nd; }
  static constexpr auto dimensions() noexcept {
    return hm3::dimensions(dimension());
  }

  static constexpr level_idx max_level() noexcept {
    constexpr auto max = 8 * sizeof(integer_t) - Nd;
    return max / Nd;
  }

  static constexpr uint_t no_levels() noexcept { return *max_level() + 1; }

  constexpr auto levels() const noexcept {
    HM3_ASSERT(level_, "location uninitialized");
    return level() == 0_l ? boxed_ints<level_idx>(0_l, 0_l)
                          : boxed_ints<level_idx>(1_l, level() + 1_l);
  }

  void reset_bits() noexcept {
    for (auto&& d : dimensions()) { x[d] = static_cast<T>(0); }
  }

  fast() = default;
  fast(fast const&) = default;
  fast& operator=(fast const&) = default;
  fast(fast&&)  = default;
  fast& operator=(fast&&) = default;

  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{})>
  fast(std::array<U, Nd> x_, level_idx l = max_level()) : level_(l) {
    HM3_ASSERT(l <= max_level(), "");

    for (auto&& d : dimensions()) {
      HM3_ASSERT(x_[d] > 0. and x_[d] < 1., "location from non-normalized "
                                            "float (d: {}, x[d]: {}) "
                                            "out-of-range (0., 1.)",
                 d, x_[d]);
    }

    num_t scale = math::ipow(2_u, *l);
    for (auto&& d : dimensions()) { from_int_r(d, x_[d] * scale); }
  }

  fast(std::initializer_list<uint_t> ps) {
    reset_bits();
    for (auto&& p : ps) { push(p); }
  }

  // from root:
  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>())> fast(Rng&& ps) {
    reset_bits();
    for (auto&& p : ps) { push(p); }
  }

  void push(uint_t pos_in_parent) {
    ++level_;
    HM3_ASSERT(level() <= max_level(), "");
    for (auto&& d : dimensions()) {
      bit::set(x[d], *level(), bit::get(pos_in_parent, d));
    }
  }

  void push(child_pos<Nd> position_in_parent) { push(*position_in_parent); }

  uint_t operator[](level_idx l) const noexcept {
    HM3_ASSERT(l > 0_l and l <= level(), "");
    uint_t value = 0;
    for (auto&& d : dimensions()) { bit::set(value, d, bit::get(x[d], *l)); }
    return value;
  }

  auto operator()() const noexcept {
    HM3_ASSERT(level_, "location uninitialized");
    return (*level() == 0_u ? boxed_ints<level_idx>(0_u, 0_u)
                            : boxed_ints<level_idx>(1_u, *level() + 1_u))
           | view::transform([&](level_idx l) { return (*this)[l]; });
  }

  /// Reverses the bits of the location
  constexpr void reverse() noexcept {
    HM3_ASSERT(level_, "location uninitialized");
    if (level() == 0_l || level() == 1_l) { return; }
    int_t first    = 1;
    int_t last     = *level();
    int swap_count = 0;
    while (last - first > 0) {
      for (auto&& d : dimensions()) { bit::swap(x[d], first, last); }
      ++first;
      --last;
      swap_count++;
    }
  }

  integer_t to_int(uint_t d) const noexcept {
    return bit::to_int_r(x[d], static_cast<integer_t>(1),
                         static_cast<integer_t>(*level() + 1));
  }

  explicit operator std::array<integer_t, Nd>() const noexcept {
    std::array<integer_t, Nd> result;
    for (auto&& d : dimensions()) { result[d] = to_int(d); }
    return result;
  }

  explicit operator integer_t() const noexcept {
    std::array<integer_t, Nd> tmp(*this);
    tmp[0] += math::ipow(2_u, *level());
    integer_t result = 0;
    std::size_t i    = 0;
    for (auto l : view::iota(0_u, *level() + 1_u)) {
      for (auto d : dimensions()) {
        bit::set(result, i, bit::get(tmp[d], l));
        ++i;
      }
    }
    return result;
  }

  void from_int_r(uint_t d, uint_t v) noexcept {
    integer_t i = 1;
    for (auto l : levels() | view::reverse) {
      bit::set(x[d], i, bit::get(v, *l - 1));
      ++i;
    }
  }

  friend opt_this_t shift(this_t l, std::array<int_t, Nd> offset) noexcept {
    for (auto&& d : dimensions()) {
      if (bit::overflows_on_add(l.to_int(d), offset[d],
                                static_cast<integer_t>(*l.level()))) {
        return opt_this_t{};
      }
    }
    for (auto&& d : dimensions()) {
      l.x[d] = bit::to_int_r(static_cast<T>(l.to_int(d) + offset[d]),
                             static_cast<T>(0), static_cast<T>(*l.level() + 1));
    }
    return opt_this_t{l};
  }

  uint_t pop() noexcept {
    uint_t tmp = (*this)[level()];
    --level_;
    return tmp;
  }

  static constexpr this_t empty_value() noexcept {
    this_t t;
    t.level_ = level_idx{};
    return t;
  }
  static constexpr bool is_empty_value(this_t v) noexcept { return !v.level(); }

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

template <typename OStream, uint_t Nd, typename T>
OStream& operator<<(OStream& os, fast<Nd, T> const& lc) {
  using f_int_t = loc_int_t<fast<Nd, T>>;
  os << "[id: " << static_cast<f_int_t>(lc) << ", lvl: " << lc.level()
     << ", xs: {";
  std::array<f_int_t, Nd> xs(lc);
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

template <uint_t Nd, typename T>
constexpr bool operator==(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  return a.level() == 0_l ? a.level() == b.level()
                          : a.level() == b.level() and equal(a(), b());
}

template <uint_t Nd, typename T>
constexpr bool operator!=(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  return !(a == b);
}

template <uint_t Nd, typename T>
constexpr bool operator<(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  using f_int_t = loc_int_t<fast<Nd, T>>;
  return static_cast<f_int_t>(a) < static_cast<f_int_t>(b);
}

template <uint_t Nd, typename T>
constexpr bool operator<=(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  return (a == b) or (a < b);
}

template <uint_t Nd, typename T>
constexpr bool operator>(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  return !(a <= b);
}

template <uint_t Nd, typename T>
constexpr bool operator>=(fast<Nd, T> const& a, fast<Nd, T> const& b) {
  return !(a < b);
}

}  // namespace location

}  // namespace tree
}  // namespace hm3
