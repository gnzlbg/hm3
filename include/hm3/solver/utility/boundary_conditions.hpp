#pragma once
/// \file
///
/// Stores which boundary cells belong to which boundary condition
#include <hm3/solver/types.hpp>
#include <hm3/utility/inline_vector.hpp>
#include <vector>

namespace hm3 {
namespace solver {

struct bcs {
  using self                                    = bcs;
  static constexpr std::size_t max_bcs_per_cell = 2;

 private:
  /// Boundary cells of each boundary condition:
  using boundary_cells = std::vector<cell_idx>;
  std::vector<boundary_cells> cells_in_bc_;

  boundary_cells& cells_in_bc(bc_idx i) noexcept {
    HM3_ASSERT(i < bc_idx(size()),
               "boundary condition index {} is out-of-bounds [0, {})", i,
               size());
    return cells_in_bc_[*i];
  }

  boundary_cells const& cells_in_bc(bc_idx i) const noexcept {
    HM3_ASSERT(i < bc_idx(size()),
               "boundary condition index {} is out-of-bounds [0, {})", i,
               size());
    return cells_in_bc_[*i];
  }

 public:
  /// #of boundary conditions
  std::size_t size() const noexcept { return cells_in_bc_.size(); }
  /// Add a new boundary condition and return its index:
  bc_idx push() noexcept {
    bc_idx i = size();
    cells_in_bc_.push_back(boundary_cells{});
    return i;
  }
  /// Shrink to fit boundary cells:
  void shrink_to_fit() {
    for (auto&& cells : cells_in_bc_) { cells.shrink_to_fit(); }
  }
  /// Clear all boundary cells for boundary condition \p i
  void clear(bc_idx i) { cells_in_bc(i).clear(); }
  /// Clear all boundary cells of all boundary conditions
  void clear() {
    for (auto&& cells : cells_in_bc_) { cells.clear(); }
  }

 private:
  template <typename F> void for_each_bc(F&& f) noexcept {
    for (bc_idx i = 0; i != size(); ++i) { f(i); }
  }
  template <typename F> void for_each_bc(F&& f) const noexcept {
    for (bc_idx i = 0; i != size(); ++i) { f(i); }
  }

 public:
  /// Does the boundary condition \p bcidx have the cell \p cidx?
  bool has_cell(cell_idx cidx, bc_idx bcidx) const noexcept {
    return ranges::lower_bound(cells_in_bc(bcidx), cidx)
           != end(cells_in_bc(bcidx));
  }

  /// All boundary conditions that cell \p cidx is a part of:
  inline_vector<bc_idx, max_bcs_per_cell> boundary_condition(cell_idx cidx) {
    inline_vector<bc_idx, max_bcs_per_cell> bc_ids;
    for_each_bc([&](bc_idx i) {
      if (has_cell(cidx, i)) { bc_ids.push_back(i); }
    });
    return bc_ids;
  }

 private:
  /// Appends a boundary cell \p cidx to the boundary condition \p bcidx
  ///
  /// \warning unsafe, the boundary cells won't be sorted afterwards
  void push(cell_idx cidx, bc_idx bcidx) { cells_in_bc(bcidx).push_back(cidx); }

  /// Sort the cells in the boundary condition
  void sort() {
    for (auto&& bc_cells : cells_in_bc_) { ranges::sort(bc_cells); }
  }

  /// Checks that all cells in the boundary condition are unique (i.e. that
  /// there are no dupplicate cells)
  ///
  /// TODO: print exactly which cells are dupplicated within a boundary
  /// condition
  ///
  /// \warning the cells must be sorted for this to work
  void assert_unique() const noexcept {
    for_each_bc([&](bc_idx i) {
      HM3_ASSERT(end(cells_in_bc(i)) == ranges::adjacent_find(cells_in_bc(i)),
                 "boundary condition {} has non unique cells", i);
    });
  }

  template <typename UnaryPredicate>
  void remove(bc_idx i, UnaryPredicate&& up) {
    HM3_ASSERT(i >= 0 and i < size(),
               "boundary condition index {} is out-of-bounds [0, {})", i,
               size());
    action::remove_if(cells_in_bc(i), std::forward<UnaryPredicate>(up));
  }

  template <typename BinaryPredicate> void remove(BinaryPredicate&& bp) {
    for_each_bc(
     [&](bc_idx i) { remove(i, [&](cell_idx j) { return bp(j, i); }); });
  }

  struct modifier {
   protected:
    self& s_;

   public:
    modifier(self& s) : s_(s) {}

    modifier(modifier const&) = delete;
    modifier(modifier&&)      = delete;
    modifier& operator=(modifier const&) = delete;
    modifier& operator=(modifier&&) = delete;

    ~modifier() {
      // Sort all the boundary cells and assert they are unique
      s_.sort();
      s_.assert_unique();
    }
  };

  struct inserter : modifier {
    using modifier::modifier;
    using modifier::operator=;

    void insert(cell_idx c, bc_idx b) { s_.push(c, b); }
  };

  struct remover : modifier {
    using modifier::modifier;
    using modifier::operator=;
    template <typename UnaryPredicate>
    void remove(bc_idx b, UnaryPredicate&& up) {
      s_.remove(b, std::forward<UnaryPredicate>(up));
    }

    template <typename BinaryPredicate> void remove(BinaryPredicate&& bp) {
      s_.remove(std::forward<BinaryPredicate>(bp));
    }
  };

 public:
  /// Returns an inserter object that knows how to insert cells:
  inserter insert() { return {*this}; }
  /// Returns a remover object that knows how to remove cells matching a
  /// predicate:
  remover remove() { return {*this}; }

  /// Applies boundary conditions to cells:
  ///
  /// For each boundary condition, call it with all its boundary cells
  ///
  /// \note The boundary conditions are called in the order specified in the
  /// tuple, and the boundary cells will be called in ascending order, but don't
  /// rely on this.
  template <typename... BCs, typename... Args>
  void apply(std::tuple<BCs...>& bcs, Args&&... args) {
    HM3_ASSERT(bcs.size() == size(), "size mismatch: bc tuple has size {} and "
                                     "bc state has {} boundary conditions",
               bcs.size(), size());
    tuple_for_each_indexed(bcs, [&](auto idx, auto&& bc) {
      for (auto&& c : cells_in_bc(idx)) { bc.apply(c, args...); }
    });
  }
};

}  // namespace solver
}  // namespace hm3
