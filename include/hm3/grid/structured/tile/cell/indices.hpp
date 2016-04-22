#pragma once
/// \file
///
/// Indices for iterating over a square tile cells'
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/grid/structured/tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/cell/index.hpp>
#include <hm3/grid/structured/tile/cell/indexed_coordinate.hpp>
#include <hm3/utility/config/attributes.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace cell {

/// Square structured tile cell indices
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per dimension
template <dim_t Nd, tidx_t Nc>  //
struct indices : bounds<Nd, Nc> {
  using self               = indices<Nd, Nc>;
  using index              = index<Nd, Nc>;
  using coordinate         = coordinate<Nd, Nc>;
  using indexed_coordinate = indexed_coordinate<Nd, Nc>;

  /// \name Internal iterators
  ///@{

  /// Executes \p f for each cell in the tile
  template <typename F, CONCEPT_REQUIRES_(Nd == 1)>  //
  [[ HM3_FLATTEN, HM3_HOT ]] static constexpr auto for_each(F&& f) noexcept {
    constexpr auto l = self::length();
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
    for (tidx_t i = 0; i < l; ++i) {
      f(indexed_coordinate(index(i), coordinate(i)));
    }
  }

  /// Executes \p f for each cell in the tile
  template <typename F, CONCEPT_REQUIRES_(Nd == 2)>  //
  [[ HM3_FLATTEN, HM3_HOT ]] static constexpr auto for_each(F&& f) noexcept {
    constexpr auto l = self::length();
    for (tidx_t j = 0; j < l; ++j) {
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
      for (tidx_t i = 0; i < l; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
    }
  }

  /// Executes \p f for each cell in the tile
  template <typename F, CONCEPT_REQUIRES_(Nd == 3)>  //
  [[ HM3_FLATTEN, HM3_HOT ]] static constexpr auto for_each(F&& f) noexcept {
    constexpr auto l = self::length();
    for (tidx_t k = 0; k < l; ++k) {
      for (tidx_t j = 0; j < l; ++j) {
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
        for (tidx_t i = 0; i < l; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
    }
  }

  /// Asserts that from is smaller or equal to to, and that both are within the
  /// tile
  static void assert_from_to(coordinate from, coordinate to) noexcept {
    HM3_ASSERT(from, "from {} is not within the tile (length: {})", from,
               self::length());
    HM3_ASSERT(to, "to {} is not within the tile (length: {})", to,
               self::length());
    for (dim_t d = 0; d < Nd; ++d) {
      HM3_ASSERT(to[d] >= from[d], "to[{}] = {} is not >= from[{}] = {}", d,
                 to[d], d, from[d]);
    }
  }

  /// \name Iteration offsets for sub-tiles
  ///
  /// These are not used anymore but are still tested and remain here since they
  /// might be useful again in the future and for other compilers.
  ///
  /// The idea is to replace the index computation from the coordinate by
  /// linearly increasing the index using these iteration offsets. So instead
  /// of:
  ///
  /// for (tidx_t k = from[2]; k <= to[2]; ++k) {
  ///   for (tidx_t j = from[1]; j <= to[1]; ++j) {
  ///     for (tidx_t i = from[0]; i <= to[0]; ++i) {
  ///       // computes index from coordinate indices:
  ///       f(indexed_coordinate(coordinate(i, j, k)));
  ///     }
  ///   }
  /// }
  ///
  /// one writes the following instead:
  ///
  /// const auto os = offsets(from, to);
  /// tidx_t c = 0;
  /// for (tidx_t k = from[2]; k <= to[2]; ++k) {
  ///   for (tidx_t j = from[1]; j <= to[1]; ++j) {
  ///     for (tidx_t i = from[0]; i <= to[0]; ++i, ++c) {
  ///       f(indexed_coordinate(index(c), coordinate(i, j, k)));
  ///     }
  ///     c += os[1];
  ///   }
  ///   c += os[2];
  /// }
  ///
  /// which avoids the index computation from the coordinate indices.
  /// Benchmark showed that with clang 3.9 it doesn't matter what one does here.
  ///
  ///@{

  /// Iteration offsets for sub-tiles (jump between coordinate dimensions)
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr std::array<tidx_t, Nd> offsets(coordinate, coordinate) {
    return {{tidx_t{1}}};
  }

  /// Iteration offsets for sub-tiles (jump between coordinate dimensions)
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr std::array<tidx_t, Nd> offsets(coordinate from,
                                                  coordinate to) {
    return {{tidx_t{1}, tidx_t{Nc - (to[0] - from[0]) - 1}}};
  }

  /// Iteration offsets for sub-tiles (jump between coordinate dimensions)
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr std::array<tidx_t, Nd> offsets(coordinate from,
                                                  coordinate to) {
    return {{tidx_t{1}, tidx_t{Nc - (to[0] - from[0]) - 1},
             tidx_t{Nc * (Nc - (to[1] - from[1]) - 1)}}};
  }
  ///@}  // Iteration offsets for sub-tiles

  /// Executes \p f for each sub-tile (\p from, \p to)
  template <typename F, CONCEPT_REQUIRES_(Nd == 1)>  //
  [[ HM3_FLATTEN, HM3_HOT ]] static constexpr auto for_each(
   const coordinate from, const coordinate to, F&& f) noexcept {
    assert_from_to(from, to);
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
    for (tidx_t i = from[0]; i <= to[0]; ++i) {
      f(indexed_coordinate(index(i), coordinate(i)));
    }
  }

  /// Executes \p f for each sub-tile (\p from, \p to)
  template <typename F, CONCEPT_REQUIRES_(Nd == 2)>  //
  [[ HM3_FLATTEN, HM3_HOT ]] static constexpr auto for_each(
   const coordinate from, const coordinate to, F&& f) noexcept {
    assert_from_to(from, to);
    for (tidx_t j = from[1]; j <= to[1]; ++j) {
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
      for (tidx_t i = from[0]; i <= to[0]; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
    }
  }

  /// Executes \p f for each sub-tile (\p from, \p to)
  template <typename F, CONCEPT_REQUIRES_(Nd == 3)>  //
  [[ HM3_FLATTEN, HM3_HOT, HM3_ALWAYS_INLINE ]] static constexpr auto for_each(
   const coordinate from, const coordinate to, F&& f) noexcept {
    assert_from_to(from, to);
    for (tidx_t k = from[2]; k <= to[2]; ++k) {
      for (tidx_t j = from[1]; j <= to[1]; ++j) {
#ifdef HM3_COMPILER_CLANG
#pragma clang loop vectorize(enable) interleave(enable)
#endif
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
    }
  }

  /// Executes \p f for each neighbor of \p x across the neighbor manifold \p m.
  ///
  /// Optionally a gap of cells in the direction of a neighbor can be specified
  /// by \p dist.
  ///
  /// All the neighbors provided are within the tile.
  ///
  /// \pre \p x must be a valid coordinate
  template <typename F, typename Manifold>
  static constexpr void for_each_neighbor(coordinate x, Manifold m, F&& f,
                                          tidx_t dist = 1) {
    HM3_ASSERT(x, "invalid coordinate: {}", x);
    for (auto pos : m()) {
      auto x_neighbor = x;
      auto offset     = m[pos];
      for (tidx_t d = 0; d < Nd; ++d) {
        offset[d] *= static_cast<noffset_t>(dist);
      }
      x_neighbor = x_neighbor.at(offset);
      if (x_neighbor) { f(indexed_coordinate(x_neighbor)); }
    }
  }

  /// Executes \p f for each neighbor of \p x across all manifolds.
  ///
  /// Optionally a gap of cells in the direction of a neighbor can be specified
  /// by \p dist.
  ///
  /// All the neighbors provided are within the tile.
  ///
  /// \pre \p x must be a valid coordinate
  template <typename F>
  static constexpr void for_each_neighbor(coordinate x, F&& f,
                                          tidx_t dist = 1) noexcept {
    HM3_ASSERT(x, "invalid coordinate: {}", x);
    tree::for_each_neighbor_manifold<Nd>(
     [&](auto m) { for_each_neighbor(x, m, std::forward<F>(f), dist); });
  }

  /// \name Iterations over rings
  ///
  /// \note Useful for implementing iteration over halo cells or closest cell
  /// queries.
  ///
  ///@{

  static constexpr void assert_from_to_w_ring(const coordinate from,
                                              const coordinate to,
                                              const tidx_t w) noexcept {
    for (dim_t d = 0; d < Nd; ++d) {
      HM3_ASSERT((to[d] - w) > (from[d] + w),
                 "d: {}, from[{}]: {}, to[{}]: {}, w: {} | (to[{}] - w) > "
                 "(from[{}] + w): {} > {} is false => the rings cells overlap "
                 "since the width w is too big!",
                 d, d, from[d], d, to[d], w, d, d, to[d] - w, from[d] + w);
    }
  }

  template <typename F, CONCEPT_REQUIRES_(Nd == 1)>
  static constexpr void for_each_ring(const coordinate from,
                                      const coordinate to, F&& f,
                                      tidx_t w = 0) noexcept {
    assert_from_to_w_ring(from, to, w);
    for (tidx_t i = from[0]; i <= from[0] + w; ++i) {
      f(indexed_coordinate(index(i), coordinate(i)));
    }
    for (tidx_t i = to[0] - w; i <= to[0]; ++i) {
      f(indexed_coordinate(index(i), coordinate(i)));
    }
  }

  template <typename F, CONCEPT_REQUIRES_(Nd == 2)>
  static constexpr void for_each_ring(const coordinate from,
                                      const coordinate to, F&& f,
                                      tidx_t w = 0) noexcept {
    assert_from_to_w_ring(from, to, w);
    // bottom:
    for (tidx_t j = from[1]; j <= from[1] + w; ++j) {
      for (tidx_t i = from[0]; i <= to[0]; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
    }

    // sides:
    for (tidx_t j = from[1] + w + 1; j <= to[1] - w - 1; ++j) {
      for (tidx_t i = from[0]; i <= from[0] + w; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
      for (tidx_t i = to[0] - w; i <= to[0]; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
    }

    // top:
    for (tidx_t j = to[1] - w; j <= to[1]; ++j) {
      for (tidx_t i = from[0]; i <= to[0]; ++i) {
        f(indexed_coordinate(coordinate(i, j)));
      }
    }
  }

  template <typename F, CONCEPT_REQUIRES_(Nd == 3)>
  static constexpr void for_each_ring(const coordinate from,
                                      const coordinate to, F&& f,
                                      tidx_t w = 0) noexcept {
    assert_from_to_w_ring(from, to, w);
    // front:
    for (tidx_t k = from[2]; k <= from[2] + w; ++k) {
      for (tidx_t j = from[1]; j <= to[1]; ++j) {
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
    }

    // sides:
    for (tidx_t k = from[2] + w + 1; k <= to[2] - w - 1; ++k) {
      // bottom:
      for (tidx_t j = from[1]; j <= from[1] + w; ++j) {
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
      // sides:
      for (tidx_t j = from[1] + w + 1; j <= to[1] - w - 1; ++j) {
        for (tidx_t i = from[0]; i <= from[0] + w; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
        for (tidx_t i = to[0] - w; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
      // top:
      for (tidx_t j = to[1] - w; j <= to[1]; ++j) {
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
    }

    // back:
    for (tidx_t k = to[2] - w; k <= to[2]; ++k) {
      for (tidx_t j = from[1]; j <= to[1]; ++j) {
        for (tidx_t i = from[0]; i <= to[0]; ++i) {
          f(indexed_coordinate(coordinate(i, j, k)));
        }
      }
    }
  }

  ///@}  // Iterations over rings

  ///@}  // Internal iterators

  /// Finds the closest cell to \p x that satisfied the predicate \p b with an
  /// upperbound distance of \p max_distance (in number of cells away
  /// from
  /// \p x).
  ///
  /// If multiple cells with the same distance satisfy the predicate the
  /// behavior is undefined (an assertion will trigger in debug mode).
  ///
  /// TODO: implement a find closest cell method that takes a container by
  /// lvalue ref and can push multiple found cells to it.
  template <typename P>
  static constexpr indexed_coordinate closest_cell(coordinate x, P&& p,
                                                   tidx_t max_cell_distance
                                                   = self::length()) noexcept {
    tidx_t ring = 1;
    indexed_coordinate x_closest;
    tidx_t d                  = std::numeric_limits<tidx_t>::max();
    coordinate min            = x.offset_if_valid(-1);
    coordinate max            = x.offset_if_valid(1);
    tidx_t count_min_distance = 0;
    while (!x_closest and ring <= max_cell_distance) {
      for_each_ring(min, max, [&](auto&& i) {
        if (i and p(i)) {
          auto new_d = distance_square(x, i.x());
          if (new_d == d) {
            ++count_min_distance;
          } else if (new_d < d) {
            count_min_distance = 0;
            d                  = new_d;
            x_closest          = i;
          }
        }
      });
      HM3_ASSERT(count_min_distance == 0, "UB! Multiple closest cells to cell "
                                          "{} that satisfy the predicate (one "
                                          "of them is {}).",
                 x, x_closest);
      min = min.offset_if_valid(-1);
      max = max.offset_if_valid(1);
      ++ring;
    }
    return x_closest;
  }

  /// \name External iterators
  ///@{

  [[ HM3_FLATTEN, HM3_HOT, HM3_ALWAYS_INLINE ]] static constexpr auto
   all() noexcept {
    return view::iota(tidx_t{0}, self::size());
  }

  /// Sub-tile view
  ///
  /// \warning This is very slow and doesn't get vectorized.
  /// TODO: make random access?
  struct subtile_view : view_facade<subtile_view, cardinality::finite> {
    friend range_access;

    coordinate from_, to_;
    coordinate current_;
    bool done_ = false;

    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] constexpr indexed_coordinate
     get() const noexcept {
      return indexed_coordinate(current_);
    }

    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] constexpr void
     next() noexcept {
      for (dim_t d = 0; d < Nd; ++d) {
        if (current_[d] < to_[d]) {
          ++current_[d];
          return;
        } else {
          current_[d] = from_[d];
        }
      }
      done_ = true;
    }

    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] constexpr bool done() const
     noexcept {
      return done_;
      // return current_ == to_;
    }
    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] constexpr bool equal(
     subtile_view const& that) const noexcept {
      return that.current_ == current_ and that.from_ == from_
             and that.to_ == to_;
    }

   public:
    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] subtile_view() = default;
    [[ HM3_ALWAYS_INLINE, HM3_FLATTEN,
       HM3_HOT ]] constexpr subtile_view(coordinate from, coordinate to)
     : from_(std::move(from)), to_(std::move(to)), current_(from_) {}
  };

  [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]] static auto sub_tile(
   coordinate from, coordinate to) noexcept {
    return subtile_view(std::move(from), std::move(to));
  }

  ///@}  // External iterators
};

}  // namespace cell
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
