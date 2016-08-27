#pragma once
/// \file
///
/// Any Geometry to VTK writer.
#ifdef HM3_ENABLE_VTK
#include <hm3/geometry/primitive/any.hpp>
#include <hm3/utility/log.hpp>
#include <hm3/utility/vector.hpp>
#include <hm3/vis/vtk/geometry.hpp>
#include <hm3/vis/vtk/serialize.hpp>
#include <hm3/vis/vtk/vtk.hpp>

namespace hm3::geometry::io {

template <dim_t Nd>
struct vtk : dimensional<Nd> {
  using any_type     = any<Nd>;
  using storage_t    = vector<any_type>;
  using vtk_cell_idx = uint_t;

  storage_t cells;  ///< Stores a copy of all the geometries to be serialized
  // TODO: it might be "easily" possible to do this by reference?

  auto geometry(vtk_cell_idx c) const noexcept {
    HM3_ASSERT(c < cells.size(), "cell idx {} out-of-bounds [0, {})", c,
               cells.size());
    return cells[c];
  }

  // TODO: rename nodes to node_ids in the VTK serialize API
  auto nodes() const noexcept {
    return view::iota(std::size_t(0), cells.size());
  }

  template <typename F>
  auto for_each_cell(F&& f) const noexcept {
    f(nodes());
    return f;
  }

  template <typename CellData>
  void load(CellData&&) const {};

  template <typename T>
  void push_cell(T&& t) {
    cells.emplace_back(std::forward<T>(t));
  }

  template <typename Rng, CONCEPT_REQUIRES_(Range<Rng>{})>
  void push_range(Rng&& rng) {
    for (auto&& c : rng) { push_cell(c); }
  }

  template <typename T>
  void push_range(std::initializer_list<T> il) {
    for (auto&& c : il) { push_cell(c); }
  }

  void write(string const& file_name) const {
    hm3::log::serial log("geometry-serialization-to-vtk");
    ::hm3::vis::vtk::serialize(*this, file_name, log);
  }

  box<Nd> bounding_box() const noexcept {
    return bounding_volume.box(bounding_volume.aabb(cells));
  }
};

}  // namespace hm3::geometry::io
#endif  // HM3_ENABLE_VTK
