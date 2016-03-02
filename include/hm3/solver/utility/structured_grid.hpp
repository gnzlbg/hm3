#pragma once
/// \file
///
/// Structured grid utilities
#include <hm3/geometry/square.hpp>
#include <hm3/solver/types.hpp>
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/utility/bit.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/math.hpp>
#include <meta/meta.hpp>

namespace hm3 {
namespace solver {

/// Structured grid indices and iteration for square grid (same number of cells
/// per dimension)
///
/// Nd: number of spatial dimensions
/// Nic: number of internal cells per dimension
/// Nhl: number of of halo layers
///
template <uint_t Nd, uint_t Nic, uint_t Nhl>  //
struct square_structured_indices : geometry::dimensional<Nd> {
  using geometry::dimensional<Nd>::dimension;
  using geometry::dimensional<Nd>::dimensions;

  /// Coordinates e.g. {i, j, k}
  using coordinates_t       = std::array<suint_t, Nd>;
  using coordinate_offset_t = std::array<int_t, Nd>;

  /// Index: cell index + coords
  struct index_t {
    suint_t idx;
    coordinates_t x;
    friend bool operator==(index_t a, index_t b) noexcept {
      return a.idx == b.idx;
    }
    friend bool operator!=(index_t a, index_t b) noexcept { return !(a == b); }
  };

  /// Number of halo layers
  static constexpr suint_t halo_layers() noexcept { return Nhl; }
  /// Number of internal cells per length
  static constexpr suint_t internal_cells_per_length() noexcept { return Nic; }
  /// Number of cells per length
  static constexpr suint_t cells_per_length() noexcept {
    return internal_cells_per_length() + 2 * halo_layers();
  }
  /// Total number of cells
  static constexpr suint_t size() noexcept {
    return math::ipow(cells_per_length(), static_cast<suint_t>(dimension()));
  }
  /// First internal tile per length
  static constexpr suint_t internal_begin(uint_t h = 0) noexcept {
    return halo_layers() - h;
  }
  /// One past last internal tile per length
  static constexpr suint_t internal_end(uint_t h = 0) noexcept {
    return halo_layers() + internal_cells_per_length() + h;
  }
  /// First halo tile left
  static constexpr suint_t left_halos_begin() noexcept { return 0; }
  /// One past last halo tile left
  static constexpr suint_t left_halos_end() noexcept { return halo_layers(); }

  /// First halo tile right
  static constexpr suint_t right_halos_begin() noexcept {
    return internal_end();
  }
  /// One past last halo tile right
  static constexpr suint_t right_halos_end() noexcept {
    return internal_end() + halo_layers();
  }

  /// External iterators
  ///@{

  /// Total range of cells
  static constexpr auto all() noexcept { return view::iota(0_su, size()); }

  /// Internal cells
  static constexpr auto internal() noexcept {
    return all() | view::filter([&](suint_t t) { return is_internal(t); });
  }

  /// Halo cells
  static constexpr auto halo() noexcept {
    return all() | view::filter([&](suint_t t) { return is_halo(t); });
  }

  /// 1D Left-halos
  static constexpr auto left_halos_1d() noexcept {
    return view::iota(left_halos_begin(), left_halos_end());
  }
  /// 1D Right-halos
  static constexpr auto right_halos_1d() noexcept {
    return view::iota(right_halos_begin(), right_halos_end());
  }
  /// 1D Internal
  static constexpr auto internal_1d(uint_t h = 0) noexcept {
    return view::iota(internal_begin(h), internal_end(h));
  }
  /// 1D All
  static constexpr auto all_1d() noexcept {
    return view::iota(0_su, cells_per_length());
  }

  ///@}  // External iterators

  /// \name Construct indices
  ///@{

  /// Tile index from spatial coordinates
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr index_t from(coordinates_t ids) noexcept {
    HM3_ASSERT(ids[0] < cells_per_length(), "");
    return {ids[0], ids};
  }

  /// Tile index from spatial coordinates
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr index_t from(coordinates_t ids) noexcept {
    HM3_ASSERT(ids[0] < cells_per_length() and ids[1] < cells_per_length(), "");
    return {ids[0] + cells_per_length() * ids[1], ids};
  }

  /// Tile index from spatial coordinates
  CONCEPT_REQUIRES(Nd == 3)
  static index_t from(coordinates_t ids) noexcept {
    HM3_ASSERT(ids[0] < cells_per_length() and ids[1] < cells_per_length()
                and ids[2] < cells_per_length(),
               "");
    return {ids[0] + cells_per_length() * ids[1]
             + (cells_per_length() * cells_per_length()) * ids[2],
            ids};
  }

  /// Spatial coordinates from tile index
  CONCEPT_REQUIRES(Nd == 1)
  static constexpr index_t from(suint_t idx) noexcept { return {idx, {{idx}}}; }

  /// Spatial coordinates from tile index
  CONCEPT_REQUIRES(Nd == 2)
  static constexpr index_t from(suint_t idx) noexcept {
    static_assert(size() != 0, "");
    const auto j = idx / cells_per_length();
    const auto i = idx % cells_per_length();
    // or: idx - (j * cells_per_length());
    return {idx, {{i, j}}};
  }

  /// Spatial coordinates from tile index
  CONCEPT_REQUIRES(Nd == 3)
  static constexpr index_t from(suint_t idx) noexcept {
    static_assert(size() != 0, "");
    const auto i   = idx % cells_per_length();
    const auto tmp = idx / cells_per_length();
    const auto j   = tmp % cells_per_length();
    const auto k   = tmp / cells_per_length();

    return {idx, {{i, j, k}}};
  }
  ///@}  // Construct indices

  /// Is tile \p t internal?
  static constexpr bool is_internal(suint_t t) noexcept {
    auto x = from(t).x;
    for (auto d : dimensions()) {
      auto i = x[d];
      if (i < internal_begin() or i >= internal_end()) { return false; }
    }
    return true;
  }
  static constexpr bool is_internal(index_t t) noexcept {
    return is_internal(t.idx);
  }

  /// Is tile \p t halo?
  static constexpr bool is_halo(suint_t t) noexcept { return !is_internal(t); }
  static constexpr bool is_halo(index_t t) noexcept { return is_halo(t.idx); }

  /// Tile at offest \p offset from tile \p tile_idx across dimension \p d
  /// TODO: slow but works
  static constexpr index_t at(index_t i, suint_t d, sint_t offset) noexcept {
    auto tmp = static_cast<sint_t>(i.x[d]) + offset;
    HM3_ASSERT(tmp >= 0, "?");
    i.x[d] = tmp;
    return from(i.x);
  }

  static constexpr index_t at(suint_t i, suint_t d, sint_t offset) noexcept {
    return at(from(i), d, std::move(offset));
  }

  static constexpr index_t at(index_t i, coordinate_offset_t offset) noexcept {
    for (auto&& d : dimensions()) {
      auto tmp = static_cast<sint_t>(i.x[d]) + offset[d];
      HM3_ASSERT(tmp >= 0, "?");
      i.x[d] = tmp;
    }
    return from(i.x);
  }

  /// Tile at offest \p offset from tile \p tile_idx across dimension \p d
  /// TODO: slow but works
  static constexpr index_t at(suint_t idx,
                              coordinate_offset_t offset) noexcept {
    return at(from(idx), std::move(offset));
  }

  /// Internal iterators
  ///@{

  template <typename Function>
  static constexpr void for_each(Function&& f) noexcept {
    RANGES_FOR (auto&& c, all()) { f(from(c)); }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 1)>
  static constexpr void for_each_internal(Function&& f, uint_t h = 0) noexcept {
    RANGES_FOR (auto&& i, internal_1d(h)) { f(from({{i}})); }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 2)>
  static constexpr void for_each_internal(Function&& f, uint_t h = 0) noexcept {
    RANGES_FOR (auto&& j, internal_1d(h)) {
      RANGES_FOR (auto&& i, internal_1d(h)) { f(from({{i, j}})); }
    }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 3)>
  static constexpr void for_each_internal(Function&& f, uint_t h = 0) noexcept {
    RANGES_FOR (auto&& k, internal_1d(h)) {
      RANGES_FOR (auto&& j, internal_1d(h)) {
        RANGES_FOR (auto&& i, internal_1d(h)) { f(from({{i, j, k}})); }
      }
    }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 1)>
  static constexpr void for_each_halo(Function&& f) noexcept {
    RANGES_FOR (auto&& i, left_halos_1d()) { f(from({{i}})); }
    RANGES_FOR (auto&& i, right_halos_1d()) { f(from({{i}})); }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 2)>
  static constexpr void for_each_halo(Function&& f) noexcept {
    // bottom
    RANGES_FOR (auto&& j, left_halos_1d()) {
      RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j}})); }
    }

    // sides
    RANGES_FOR (auto&& j, internal_1d()) {
      RANGES_FOR (auto&& i, left_halos_1d()) { f(from({{i, j}})); }
      RANGES_FOR (auto&& i, right_halos_1d()) { f(from({{i, j}})); }
    }

    // top
    RANGES_FOR (auto&& j, right_halos_1d()) {
      RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j}})); }
    }
  }

  template <typename Function, CONCEPT_REQUIRES_(Nd == 3)>
  static constexpr void for_each_halo(Function&& f) noexcept {
    /// front:
    RANGES_FOR (auto&& k, left_halos_1d()) {
      RANGES_FOR (auto&& j, all_1d()) {
        RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j, k}})); }
      }
    }

    /// back:
    RANGES_FOR (auto&& k, right_halos_1d()) {
      RANGES_FOR (auto&& j, all_1d()) {
        RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j, k}})); }
      }
    }

    /// bottom:
    RANGES_FOR (auto&& k, internal_1d()) {
      RANGES_FOR (auto&& j, left_halos_1d()) {
        RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j, k}})); }
      }
    }

    /// top:
    RANGES_FOR (auto&& k, internal_1d()) {
      RANGES_FOR (auto&& j, right_halos_1d()) {
        RANGES_FOR (auto&& i, all_1d()) { f(from({{i, j, k}})); }
      }
    }

    /// side left:
    RANGES_FOR (auto&& k, internal_1d()) {
      RANGES_FOR (auto&& j, internal_1d()) {
        RANGES_FOR (auto&& i, left_halos_1d()) { f(from({{i, j, k}})); }
      }
    }
    RANGES_FOR (auto&& k, internal_1d()) {
      RANGES_FOR (auto&& j, internal_1d()) {
        RANGES_FOR (auto&& i, right_halos_1d()) { f(from({{i, j, k}})); }
      }
    }
    /// side right:
    RANGES_FOR (auto&& k, internal_1d()) {
      RANGES_FOR (auto&& j, internal_1d()) {
        RANGES_FOR (auto&& i, right_halos_1d()) { f(from({{i, j, k}})); }
      }
    }
  }
  ///@}  // Internal iterators

  // o is the distance in #of cells from cell across each neighbor direction
  template <typename F>
  void for_each_neighbor(index_t cell, F&& f, uint_t o = 1) const noexcept {
    tree::for_each_neighbor_manifold<Nd>([&](auto m) {
      for (auto pos : m()) {
        auto neighbor_x = cell.x;
        auto offset     = m[pos];
        for (auto d : dimensions()) {
          neighbor_x[d] = static_cast<sint_t>(neighbor_x[d]) + offset[d] * o;
          if (neighbor_x[d] <= 0 or neighbor_x[d] >= cells_per_length()) {
            goto next_neighbor;  // overflow, skip this neighbor
          }
        }
        {
          auto neighbor_idx = from(neighbor_x);
          f(neighbor_idx);
        }
      next_neighbor:
        continue;
      }
    });
  }

  constexpr index_t closest_internal_cell(index_t halo) const noexcept {
    HM3_ASSERT(is_halo(halo), "cell {} is not a halo", halo.idx);
    sint_t o         = 1;
    index_t neighbor = halo;

    while (o <= Nhl) {
      tree::for_each_neighbor_manifold<Nd>([&](auto m) {
        if (neighbor != halo) { return; }
        for (auto pos : m()) {
          auto neighbor_x = halo.x;
          auto offset     = m[pos];
          for (auto d : dimensions()) {
            if (bit::overflows_on_add(neighbor_x[d], offset[d] * o)) {
              continue;
            }
            neighbor_x[d] = static_cast<sint_t>(neighbor_x[d]) + offset[d] * o;
            if (neighbor_x[d] <= 0 or neighbor_x[d] >= cells_per_length()) {
              goto next_neighbor;  // overflow, skip this neighbor
            }
          }
          {
            auto neighbor_idx = from(neighbor_x);
            if (is_internal(neighbor_idx)) { neighbor = neighbor_idx; }
          }
        next_neighbor:
          continue;
        }
      });
      if (neighbor != halo) { return neighbor; }
      o += 1;
    }
    HM3_FATAL_ERROR("closest internal cell of halo {} not found", halo.idx);
  }
};

template <uint_t Nd, uint_t Nic, uint_t Nhl>  //
struct square_structured_grid : square_structured_indices<Nd, Nic, Nhl> {
  static_assert(Nic >= Nhl, "");
  using indices       = square_structured_indices<Nd, Nic, Nhl>;
  using index_t       = typename indices::index_t;
  using coordinates_t = typename indices::coordinates_t;
  using indices::dimension;
  using indices::dimensions;
  using indices::size;
  using indices::at;
  using indices::cells_per_length;
  using indices::is_internal;

  using point_t  = geometry::point<Nd>;
  using square_t = geometry::square<Nd>;

  /// Square structured grid geometry
  square_t bounding_box_;
  /// Cell length
  num_t cell_length_;
  /// Coordinates of the first cell
  point_t x_first_cell_;

  constexpr auto grid_length() const noexcept {
    return geometry::length(bounding_box_);
  }

  constexpr num_t const& cell_length() const noexcept { return cell_length_; }
  constexpr num_t cell_volume() const noexcept {
    return std::pow(cell_length_, Nd);
  }

  constexpr num_t cell_surface_area() const noexcept {
    return Nd == 1 ? 1.0 : std::pow(cell_length_, Nd - 1);
  }

  /// Center coordinates of cell at index \p i
  constexpr point_t centroid(index_t i) const noexcept {
    auto xc = x_first_cell_;
    for (auto&& d : dimensions()) { xc(d) += i.x[d] * cell_length(); }
    return xc;
  }

  /// Center coordinates of cell at index \p i
  constexpr point_t centroid(uint_t idx) const noexcept {
    return centroid(indices::from(idx));
  }

  /// Geometry of cell at index \p i
  constexpr square_t geometry(index_t i) const noexcept {
    return {centroid(std::move(i)), cell_length()};
  }

  /// Geometry of cell at index \p idx
  constexpr square_t geometry(uint_t idx) const noexcept {
    return geometry(indices::from(idx));
  }

  /// Compute the cell length from the length of the block
  static constexpr num_t compute_cell_length(num_t block_length) noexcept {
    return block_length / indices::internal_cells_per_length();
  }

  /// Computes first cell coordinates (idx = 0) from bounding box coordinates
  static constexpr point_t compute_first_cell_coordinates(
   square_t bbox) noexcept {
    auto x_min  = geometry::bounds(bbox).min;
    auto cell_l = compute_cell_length(geometry::length(bbox));
    for (auto&& d : dimensions()) {
      x_min(d) += cell_l / 2. - cell_l * indices::halo_layers();
    }
    return x_min;
  }

  /// Block's bounding box
  constexpr square_t bounding_box() const noexcept { return bounding_box_; }

  /// Lies point \p x in grid?
  constexpr bool in_grid(point_t x) const noexcept {
    return geometry::contains(bounding_box_, x);
  }

  /// Index of cell containing point x
  constexpr index_t at(point_t x) const noexcept {
    HM3_ASSERT(in_grid(x), "point x is not in block");
    // std::cout << "x: " << x() << std::endl;

    // normalize x:
    auto x_min = x_first_cell_;
    x() -= x_min();
    x() /= cell_length();
    // std::cout << "x_normalized: " << x() << std::endl;

    index_t i;
    // std::cout << "x_int: ";
    for (auto&& d : dimensions()) {
      // auto v = floor(x[d]);
      auto v = x[d];
      i.x[d] = std::lround(v);  // );
      // std::cout << i.x[d] << "( " << x[d] << ", " << floor(x[d]) <<  " )"
      //           << " ";
      HM3_ASSERT(i.x[d] >= 0 and i.x[d] < cells_per_length(), "");
    }
    //    std::cout << std::endl;

    return this->from(i.x);
  }

  /// Index of non-halo cell containing point x
  constexpr index_t at_nh(point_t x) const noexcept {
    HM3_ASSERT(in_grid(x), "point x is not in block");
    auto i = at(x);
    HM3_ASSERT(is_internal(i), "?");
    return i;
  }

  constexpr square_structured_grid()                              = default;
  constexpr square_structured_grid(square_structured_grid const&) = default;
  constexpr square_structured_grid& operator=(square_structured_grid const&)
   = default;
  constexpr square_structured_grid(square_structured_grid&&) = default;
  constexpr square_structured_grid& operator=(square_structured_grid&&)
   = default;

  constexpr square_structured_grid(geometry::square<Nd> bbox) {
    reinitialize(std::move(bbox));
  }

  constexpr void reinitialize(geometry::square<Nd> bbox) noexcept {
    bounding_box_ = std::move(bbox);
    cell_length_  = compute_cell_length(geometry::length(bounding_box_));
    x_first_cell_ = compute_first_cell_coordinates(bounding_box_);
    HM3_ASSERT(cell_length() > 0.,
               "zero cell length in square structured grid with bbox: {}",
               bounding_box_);
  }
};

}  // namespace solver
}  // namespace hm3
