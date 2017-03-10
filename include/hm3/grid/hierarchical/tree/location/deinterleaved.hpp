#pragma once
/// \file
///
/// Deinterleaved location implementation
///
/// Slower and requires 4x more memory than the interleaved location
/// implementation. It might be possible to make this faster by using BMI2.
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/math/core.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::tree::location {

/// Deinterleaved location code for a tree node
///
/// \tparam Ad          number of spatial dimensions of the tree
/// \tparam MortonIdxT  internal storage of the location code, stored as a
///                     an interleaved Morton index in an unsigned integral.
///
/// A location code stores the position of a node within the tree by storing
/// the traversal from the root of the tree to the node as a sequence of bits.
///
/// This location code stores the components of the morton index deinterleaved
/// in an array. That is, the deinterleaved components (x0,x1,x2,...),
/// (y0,y1,y2,...), form the morton index x0y0z0x1y1z1x2y2z2...
///
/// \note The deinterleaved representation requires ad+1 words of storage while
/// the interleaved representation requires only 1 word of storage. While this
/// representation is tested, it should probably not be used for anything in
/// practice.
///
/// TODO: Because this is less used it is less documented than the
/// `interleaved` location implementation but the API "should" be the same.
///
template <dim_t Ad, typename MortonIdxT = uint_t>
struct deinterleaved : geometry::with_ambient_dimension<Ad> {
  using self     = deinterleaved<Ad, MortonIdxT>;
  using opt_self = compact_optional<self>;

  using morton_idx_t = MortonIdxT;
  using morton_x_t   = array<morton_idx_t, Ad>;

  /// Required to model the CompactOptional concept
  using value_type     = self;
  using storage_type   = self;
  using reference_type = self const&;

  /// Deinterleaved morton coordinates
  morton_x_t x_;
  /// Node level
  level_idx level_ = 0;

  using geometry::with_ambient_dimension<Ad>::ambient_dimension;
  using geometry::with_ambient_dimension<Ad>::ambient_dimensions;

  /// \name Node level
  ///@{

  /// Maximum number of levels that can be stored in the location code
  ///
  /// This is limited by the loss-less conversion to the morton_idx_t type.
  static constexpr lidx_t max_no_levels() noexcept {
    constexpr auto no_bits  = bit::width<morton_idx_t>;
    constexpr auto loc_size = Ad;
    return math::floor((static_cast<num_t>(no_bits - loc_size))
                       / static_cast<num_t>(loc_size));
  }

  /// Maximum level that can be returned by the location code
  ///
  /// The root level is zero so this is just the maximum number of levels - 1.
  static constexpr level_idx max_level() noexcept {
    return max_no_levels() - 1;
  }

  /// Level of the node
  constexpr level_idx level() const noexcept { return level_; }

  /// Range of node levels: [1, `level()`]
  constexpr auto levels() const noexcept {
    HM3_ASSERT(level_, "location uninitialized");
    return level() == 0_l ? boxed_ints<level_idx>(0_l, 0_l)
                          : boxed_ints<level_idx>(1_l, level() + 1_l);
  }

  ///@}  // Node level

  /// \name Mutation
  ///@{

  /// Change location to the current node's children located at position \p
  /// position_in_parent.
  constexpr void push(cpidx_t pos_in_parent) {
    ++level_;
    HM3_ASSERT(level() <= max_level(), "");
    for (auto&& d : ambient_dimensions()) {
      bit::set(x_[d], *level(), bit::get(pos_in_parent, d));
    }
  }

  /// Change location to the current node's children located at position \p
  /// position_in_parent.
  constexpr void push(child_pos<Ad> position_in_parent) {
    push(*position_in_parent);
  }

  /// Change location to the parent of the current node.
  constexpr cpidx_t pop() noexcept {
    cpidx_t tmp = (*this)[level()];
    --level_;
    return tmp;
  }

 private:
  /// Resets the morton index
  constexpr void reset_bits() noexcept {
    for (auto&& d : ambient_dimensions()) {
      x_[d] = static_cast<morton_idx_t>(0);
    }
    level_ = 0;
  }

 public:
  /// Reverses the bits of the location
  constexpr void reverse() noexcept {
    HM3_ASSERT(level_, "location uninitialized");

    if (level() == 0_l || level() == 1_l) { return; }
    int_t first    = 1;
    int_t last     = *level();
    int swap_count = 0;
    while (last - first > 0) {
      for (auto&& d : ambient_dimensions()) { bit::swap(x_[d], first, last); }
      ++first;
      --last;
      swap_count++;
    }
  }

 private:
  morton_idx_t to_int(uint_t d) const noexcept {
    return bit::to_int_r(x_[d], static_cast<morton_idx_t>(1),
                         static_cast<morton_idx_t>(*level() + 1));
  }

  void from_int_r(uint_t d, uint_t v) noexcept {
    morton_idx_t i = 1;
    for (auto l : levels() | view::reverse) {
      bit::set(x_[d], i, bit::get(v, *l - 1));
      ++i;
    }
  }

 public:
  ///@}  // Mutation

  /// \name Access
  ///@{

  /// Does the location point to a valid node?
  constexpr bool valid() const noexcept { return !level_; }

  /// Position in parent of the node at distance \p level_ from the root-node to
  /// the node stored in the location.
  constexpr cpidx_t operator[](level_idx l) const noexcept {
    HM3_ASSERT(l > 0_l and l <= level(), "");
    cpidx_t value = 0;
    for (auto&& d : ambient_dimensions()) {
      bit::set(value, d, bit::get(x_[d], *l));
    }
    return value;
  }

  /// Position in parent of all nodes between the root node and the node stored
  /// in the location.
  constexpr auto operator()() const noexcept {
    HM3_ASSERT(level_, "location uninitialized");
    return (*level() == 0_u ? boxed_ints<level_idx>(0_u, 0_u)
                            : boxed_ints<level_idx>(1_u, *level() + 1_u))
           | view::transform([&](level_idx l) { return (*this)[l]; });
  }

  /// Explicit conversion operator to the underyling integer type as the
  /// interleaved Morton indices of the location
  constexpr explicit operator morton_idx_t() const noexcept {
    interleaved<Ad, MortonIdxT> tmp((*this)());
    return static_cast<morton_idx_t>(tmp);
  }

  /// Explicit conversion operator to the deinterleaved Morton coordinates
  constexpr explicit operator morton_x_t() const noexcept {
    morton_x_t result;
    for (auto&& d : ambient_dimensions()) { result[d] = to_int(d); }
    return result;
  }

  /// Returns the Morton index of the location
  morton_idx_t morton_idx() const noexcept { return morton_idx_t(*this); }

  /// Returns the deinterleaved Morton coordinates of the location
  morton_x_t morton_x() const noexcept { return morton_x_t(*this); }

  ///@} // Access

  constexpr deinterleaved()                     = default;
  constexpr deinterleaved(deinterleaved const&) = default;
  constexpr deinterleaved& operator=(deinterleaved const&) = default;
  constexpr deinterleaved(deinterleaved&&)                 = default;
  constexpr deinterleaved& operator=(deinterleaved&&) = default;
  ~deinterleaved()                                    = default;

  /// Constructs a location from deinterleaved Morton coordinates \p x_ up to
  /// a level \p l.
  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{})>
  deinterleaved(array<U, Ad> x, level_idx l = max_level()) : level_(l) {
    HM3_ASSERT(l <= max_level(), "");

    for (auto&& d : ambient_dimensions()) {
      HM3_ASSERT(x[d] > 0. and x[d] < 1.,
                 "location from non-normalized float "
                 "(d: {}, x[d]: {}) out-of-range (0., "
                 "1.)",
                 d, x[d]);
    }

    num_t scale = math::ipow(2_su, *l);
    for (auto&& d : ambient_dimensions()) { from_int_r(d, x[d] * scale); }
  }

  /// Constructs a location code from a \p list of children positions in
  /// descending order from the root node.
  deinterleaved(std::initializer_list<cpidx_t> ps) {
    reset_bits();
    for (auto&& p : ps) { push(p); }
  }

  /// Constructs a location code from a range of children positions in
  /// descending order from the root node.
  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>())>
  deinterleaved(Rng&& ps) {
    reset_bits();
    for (auto&& p : ps) { push(p); }
  }

 public:
  /// Shifst the location \p t by an spatial \p offset.
  friend opt_self shift(self l, offset_t<Ad> offset) noexcept {
    for (auto&& d : ambient_dimensions()) {
      if (bit::overflows_on_add(l.to_int(d), offset[d],
                                static_cast<morton_idx_t>(*l.level()))) {
        return opt_self{};
      }
    }
    for (auto&& d : ambient_dimensions()) {
      l.x_[d] = bit::to_int_r(
       static_cast<morton_idx_t>(static_cast<int_t>(l.to_int(d)) + offset[d]),
       static_cast<morton_idx_t>(0), static_cast<morton_idx_t>(*l.level() + 1));
    }
    return opt_self{l};
  }

  /// \name CompactOptional invalid state
  ///@{
  static constexpr self empty_value() noexcept {
    self t;
    t.level_ = level_idx{};
    return t;
  }
  static constexpr bool is_empty_value(self v) noexcept { return !v.level(); }

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
  ///@} // CompactOptional invalid state

  static constexpr self min() noexcept {
    self l;
    while (*l.level() < *l.max_level()) { l.push(0); }
    return l;
  }
};

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator==(deinterleaved<Ad, MortonIdxT> const& a,
                          deinterleaved<Ad, MortonIdxT> const& b) {
  return a.level() == 0_l ? a.level() == b.level()
                          : a.level() == b.level() and equal(a(), b());
}

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator!=(deinterleaved<Ad, MortonIdxT> const& a,
                          deinterleaved<Ad, MortonIdxT> const& b) {
  return !(a == b);
}

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator<(deinterleaved<Ad, MortonIdxT> const& a,
                         deinterleaved<Ad, MortonIdxT> const& b) {
  using f_int_t = morton_idx_t<deinterleaved<Ad, MortonIdxT>>;
  return static_cast<f_int_t>(a) < static_cast<f_int_t>(b);
}

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator<=(deinterleaved<Ad, MortonIdxT> const& a,
                          deinterleaved<Ad, MortonIdxT> const& b) {
  return (a == b) or (a < b);
}

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator>(deinterleaved<Ad, MortonIdxT> const& a,
                         deinterleaved<Ad, MortonIdxT> const& b) {
  return !(a <= b);
}

template <dim_t Ad, typename MortonIdxT>
constexpr bool operator>=(deinterleaved<Ad, MortonIdxT> const& a,
                          deinterleaved<Ad, MortonIdxT> const& b) {
  return !(a < b);
}

}  // namespace hm3::tree::location
