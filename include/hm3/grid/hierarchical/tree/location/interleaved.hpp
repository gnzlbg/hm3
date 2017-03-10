#pragma once
/// \file
///
/// Interleaved location
#include <hm3/geometry/fwd.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <type_traits>

namespace hm3::tree::location {

/// Interleaved location code for a tree node
///
/// \tparam Ad          number of spatial dimensions of the tree
/// \tparam MortonIdxT  internal storage of the location code, stored as a
///                     an interleaved Morton index in an unsigned integral.
///
/// A location code stores the position of a node within the tree by storing
/// the traversal from the root of the tree to the node as a sequence of bits.
///
/// This location code uses a single integer type in which it stores interleaved
/// Morton coordinates (xyzxyzxyz... that is, the Morton index).
///
template <dim_t Ad, typename MortonIdxT = uint_t>
struct interleaved : geometry::with_ambient_dimension<Ad> {
  using self = interleaved<Ad, MortonIdxT>;

  /// Underlying storage type for the Morton index
  using morton_idx_t = MortonIdxT;

  /// Deinterleaved Morton coordinates.
  ///
  /// Each element in the array represents a Cartesian spatial direction.
  ///
  /// Each bit in each direction refers to a level "l" within the tree (the
  /// first bit is the 0th level, the "l"th bit is the lth level).
  ///
  /// The bits encode the relative positions of a node at a level "l" with
  /// respect to its parent along a Cartesian spatial direction.
  ///
  /// That is for the coordinate "i" the bit "l" denotes that the node at level
  /// "l" from the root node encoded in the location has a positive (true) or
  /// negative (false) relative position to its parent along the coordinate "i".
  ///
  using morton_x_t = array<morton_idx_t, Ad>;

  /// Required to model the CompactOptional concept
  using value_type     = self;
  using storage_type   = self;
  using reference_type = self const&;

  static_assert(
   UnsignedIntegral<morton_idx_t>{},
   "location::interleaved storage must be an unsigned integral type");

  /// Underlying storage of the location code default constructed to the root
  /// node of the tree.
  morton_idx_t value = 1;

  using geometry::with_ambient_dimension<Ad>::ambient_dimension;
  using geometry::with_ambient_dimension<Ad>::ambient_dimensions;

  /// \name Node level
  ///@{

  /// Maximum number of levels that can be stored in the location code
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
  ///
  /// \pre The location must point to a valid node (see `valid()`).
  constexpr level_idx level() const noexcept {
    HM3_ASSERT(valid(),
               "trying to obtain the level of an uninitialized location code");
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
    return (bit::width<morton_idx_t> - 1 - bit::clz(value)) / Ad;
  }

  /// Range of node levels: [1, `level()`]
  constexpr auto levels() const noexcept {
    HM3_ASSERT(valid(), "invalid node location");
    HM3_ASSERT(level() > 0_l, "level out of bounds");
    return boxed_ints<level_idx>(1_l, level() + 1_l);
  }

  ///@}  // Node level

  /// \name Mutation
  ///@{

  /// Change location to the current node's children located at position \p
  /// position_in_parent.
  ///
  /// \pre \p position_in_parent must be in bounds: [0, no_children(Ad)).
  ///
  /// \pre The node level cannot be the `max_level()` (since children of nodes
  /// at the max_level() cannot be stored within this location).
  ///
  /// \pre The location must point to a valid node.
  constexpr void push(cpidx_t position_in_parent) noexcept {
    HM3_ASSERT(valid(), "trying to push a children to an invalid node");
    HM3_ASSERT(position_in_parent < no_children(Ad),
               "position in parent {} out-of-bounds [0, {}) (Ad: {})",
               position_in_parent, no_children(Ad), Ad);
    HM3_ASSERT(level() != max_level(),
               "location \"full\": level equals max_level {}", max_level());
    value = (value << Ad) + position_in_parent;
  }

  /// Change location to the current node's children located at position \p
  /// position_in_parent.
  ///
  /// \pre The \p position_in_parent must be valid.
  ///
  /// \pre The node level cannot be the `max_level()` (since children of nodes
  /// at the max_level() cannot be stored within this location).
  ///
  /// \pre The location must point to a valid node
  constexpr void push(child_pos<Ad> position_in_parent) {
    push(*position_in_parent);
  }

  static constexpr self min() noexcept {
    self l;
    while (*l.level() < *l.max_level()) { l.push(0); }
    return l;
  }

  /// Change location to the parent of the current node.
  ///
  /// \return The position in parent of the current node.
  ///
  /// \pre The node is not the root node (the root node is not a child of any
  /// other node so it cannot be `pop()`ed).
  constexpr cpidx_t pop() noexcept {
    HM3_ASSERT(level() > level_idx{0_u},
               "cannot pop root-node from location code");
    cpidx_t tmp = (*this)[level()];
    value >>= Ad;
    return tmp;
  }

  /// Reverses the order of positions in parent of the location code.
  ///
  /// That is, this changes a root-to-node encoding to a node-to-root encoding
  /// and vice-versa.
  void reverse() {
    interleaved other;
    for (auto l : (*this)() | view::reverse) { other.push(l); }
    (*this) = other;
  }

  ///@}  // Mutation

  /// \name Access
  ///@{

  /// Does the location point to a valid node?
  constexpr bool valid() const noexcept { return value != 0; }

  /// Position in parent of the node at distance \p level_ from the root-node to
  /// the node stored in the location.
  ///
  /// \pre \p level_ must be in range (0, `level()`]. It cannot be 0 since the
  /// root node is not the child of any node.
  constexpr cpidx_t operator[](const level_idx level_) const noexcept {
    HM3_ASSERT(level_ > 0_l and level_ <= level(),
               "level {} out-of-bounds [1, {})", level_, level());
    morton_idx_t mask             = no_children(Ad) - 1;
    const morton_idx_t mask_shift = (*(level() - level_)) * Ad;
    mask <<= mask_shift;
    mask &= value;
    mask >>= mask_shift;
    return mask;
  }

  /// Position in parent of all nodes between the root node and the node stored
  /// in the location.
  constexpr auto operator()() const noexcept {
    const auto l = level();
    return (l == 0_l ? boxed_ints<level_idx>(0_l, 0_l) : levels())
           | view::transform([=](level_idx l_i) { return (*this)[*l_i]; });
  }

 public:
  /// Returns the Morton index of the location
  constexpr morton_idx_t morton_idx() const noexcept { return value; }

  /// Returns the deinterleaved Morton coordinates of the location
  constexpr morton_x_t morton_x() const noexcept {
    return decode(value, level());
  }

  /// Explicit conversion operator to the underyling integer type as the
  /// interleaved Morton indices of the location
  constexpr explicit operator morton_idx_t() const noexcept {
    return morton_idx();
  }

  /// Explicit conversion operator to the deinterleaved Morton coordinates
  constexpr explicit operator morton_x_t() const noexcept { return morton_x(); }

  ///@} // Access

  /// \name Morton index/coordinates encoding/decoding
  ///
  /// Encoding/decoding is <=4cycles on CPUs >= Haswell with BMI>=2.0 but more
  /// expensive on older CPUs. See `bit::morton` encoding/decoding utilities
  /// which use parallel bit deposit/extract instructions if available.
  ///
  ///@{

 private:
  /// Sets the \p l -th bit of the 0th coordinate of \p xs to true.
  static constexpr morton_x_t set_level(morton_x_t xs, level_idx l) noexcept {
    // TODO: explain why the level is only set for the first coordinate
    bit::set(xs[0], *l, true);
    return xs;
  }

  /// Encodes the deinterleaved Morton coordinates \p xs of a node at level \p l
  /// into a Morton index.
  static constexpr morton_idx_t encode(morton_x_t xs, level_idx l) noexcept {
    // TODO: explain why set level is necessary
    return bit::morton::encode(set_level(xs, l));
  }

  /// Sets the \p l -th bit of the 0th coordinate of \p xs to false.
  static constexpr morton_x_t remove_level(morton_x_t xs,
                                           level_idx l) noexcept {
    // TODO: explain why the level is only removed for the first coordinate
    bit::set(xs[0], *l, false);
    return xs;
  }

  /// Decodes a Morton index \p v into deinterleaved Morton coordinates up to
  /// level \p l.
  static constexpr morton_x_t decode(morton_idx_t v, level_idx l) noexcept {
    return remove_level(bit::morton::decode(v, morton_x_t{}), l);
  }

 public:
  ///@}  // Morton index/coordinates encoding/decoding

  constexpr interleaved()                   = default;
  constexpr interleaved(interleaved const&) = default;
  constexpr interleaved& operator=(interleaved const&) = default;
  constexpr interleaved(interleaved&&)                 = default;
  constexpr interleaved& operator=(interleaved&&) = default;
  ~interleaved()                                  = default;

  /// Constructs a location code from a \p list of children positions in
  /// descending order from the root node.
  interleaved(std::initializer_list<cpidx_t> list) : interleaved() {
    for (auto&& p : list) { push(p); }
  }

  /// Constructs a location code from a range of children positions in
  /// descending order from the root node.
  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>())>
  interleaved(Rng&& positions_in_parent) : interleaved() {
    for (auto&& p : positions_in_parent) { push(p); }
  }

  /// Constructs a location from deinterleaved Morton coordinates \p x_ up to
  /// a level \p l.
  interleaved(morton_x_t xs, level_idx l = max_level()) : value(encode(xs, l)) {
    HM3_ASSERT(l <= max_level(), "level {} out-of-bounds [1, {}]", l,
               max_level());
  }

  /// Constructs a location from deinterleaved Morton coordinates \p x_ up to
  /// a level \p l.
  ///
  /// By default this is "as lossless as possible", that is the coordinates are
  /// stored up to the maximum level that can be stored within the location
  /// code.
  template <typename U, CONCEPT_REQUIRES_(std::is_floating_point<U>{})>
  interleaved(array<U, Ad> x_, level_idx l = max_level()) {
    HM3_ASSERT(l <= max_level(), "level {} out-of-bounds [1, {}]", l,
               max_level());

    for (auto&& d : ambient_dimensions()) {
      HM3_ASSERT(x_[d] > 0. and x_[d] < 1.,
                 "location from non-normalized "
                 "float (d: {}, x[d]: {}) "
                 "out-of-range (0., 1.)",
                 d, x_[d]);
    }

    num_t scale = math::ipow(lidx_t{2}, *l);
    array<morton_idx_t, Ad> tmp;
    for (auto&& d : ambient_dimensions()) { tmp[d] = x_[d] * scale; }
    value = encode(tmp, l);

    while (level() > l) { pop(); }
  }

  /// \name CompactOptional invalid state
  ///@{
  static constexpr self empty_value() noexcept {
    self t;
    t.value = morton_idx_t{0};
    return t;
  }
  static constexpr bool is_empty_value(self v) noexcept {
    return v.value == morton_idx_t{0};
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
  ///@} // CompactOptional invalid state
};

/// Shifst the location \p t by an spatial \p offset.
///
/// The coordinates of the offset are in "nodes at `t.level()`"-units.
///
/// Algorithm: O(1)
/// 1. The location \p t is decoded into deinterleaved Morton coordinates.
/// 2. The coordinates are shifted by the \p offset.
/// 3. The shifted coordinates are re-encoded into a Morton index.
///
/// If the resulting node lies outside of the tree bounding box this returns
/// None, otherwise it returns Some(Location).
///
/// \note This is how e.g. neighbors are found within the tree.
template <dim_t Ad, typename Int>
compact_optional<interleaved<Ad, Int>> shift(interleaved<Ad, Int> t,
                                             offset_t<Ad> offset) noexcept {
  using loc_t        = interleaved<Ad, Int>;
  using morton_idx_t = typename loc_t::morton_idx_t;
  auto level         = t.level();
  auto ilevel        = static_cast<morton_idx_t>(*level);
  auto xs            = t.morton_x();  // TODO: loc_t::decode(t.value, ilevel); ?
  if (ranges::none_of(ambient_dimension[t], [&](auto&& d) {
        return bit::overflows_on_add(xs[d], offset[d], ilevel);
      })) {
    for (auto&& d : ambient_dimension[t]) {
      xs[d] = math::add_signed_to_unsigned(offset[d], xs[d]);
    }
    t = interleaved<Ad, Int>(xs, level);  //.value = loc_t::encode(xs, level);

    HM3_ASSERT(t.valid(), "logic error: invalid t with value {}", t.value);
    return compact_optional<loc_t>{t};
  }
  return compact_optional<loc_t>{};
}

template <dim_t Ad, typename T>
constexpr bool operator==(interleaved<Ad, T> const& a,
                          interleaved<Ad, T> const& b) noexcept {
  return a.value == b.value;
}

template <dim_t Ad, typename T>
constexpr bool operator!=(interleaved<Ad, T> const& a,
                          interleaved<Ad, T> const& b) noexcept {
  return !(a == b);
}

template <dim_t Ad, typename T>
constexpr bool operator<(interleaved<Ad, T> const& a,
                         interleaved<Ad, T> const& b) noexcept {
  return a.value < b.value;
}

template <dim_t Ad, typename T>
constexpr bool operator<=(interleaved<Ad, T> const& a,
                          interleaved<Ad, T> const& b) noexcept {
  return (a == b) or (a < b);
}

template <dim_t Ad, typename T>
constexpr bool operator>(interleaved<Ad, T> const& a,
                         interleaved<Ad, T> const& b) noexcept {
  return !(a.value <= b.value);
}

template <dim_t Ad, typename T>
constexpr bool operator>=(interleaved<Ad, T> const& a,
                          interleaved<Ad, T> const& b) noexcept {
  return !(a < b);
}

static_assert(std::is_standard_layout<interleaved<1_u>>{}, "");
static_assert(std::is_literal_type<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_constructible<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_default_constructible<interleaved<1_u>>{}, "");
#ifndef __GLIBCXX__  // libstdc++-4.9 doesn't provide these
static_assert(std::is_trivially_copy_constructible<interleaved<1_u>>{}, "");
static_assert(std::is_trivially_move_constructible<interleaved<1_u>>{}, "");
static_assert(
 std::is_trivially_assignable<interleaved<1_u>, interleaved<1_u>>{}, "");
static_assert(std::is_trivially_copy_assignable<interleaved<1_u>>{}, "");
static_assert(std::is_trivially_move_assignable<interleaved<1_u>>{}, "");
#endif
static_assert(std::is_nothrow_copy_constructible<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_move_constructible<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_assignable<interleaved<1_u>, interleaved<1_u>>{},
              "");
static_assert(std::is_nothrow_copy_assignable<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_move_assignable<interleaved<1_u>>{}, "");
static_assert(std::is_nothrow_destructible<interleaved<1_u>>{}, "");
static_assert(std::is_trivially_destructible<interleaved<1_u>>{}, "");

}  // namespace hm3::tree::location
