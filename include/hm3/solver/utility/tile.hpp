#pragma once
/// \file
///
///
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/solver/geometry/types.hpp>
#include <hm3/solver/types.hpp>

namespace hm3::solver {

template <typename B, typename T, typename F>
void call_as(T&& t, F&& f) {
  using base_type_ref = std::add_lvalue_reference_t<
   std::conditional_t<std::is_const<T>{}, std::add_const<B>, B>>;
  f(static_cast<base_type_ref>(t));
}

template <typename B, typename... Bs, typename T, typename F>
auto call_as(T&& t, F&& f) -> std::enable_if_t<(sizeof...(Bs) > 0), void> {
  call_as<B>(t, f);
  call_as<Bs...>(t, f);
}

template <typename BoxGrid, typename... Vars>
struct tile : BoxGrid, Vars... {
  using grid_t = BoxGrid;
  using bbox_t = hg::box<BoxGrid::ambient_dimension()>;
  /// Data
  ///
  /// \note The rest of the data is stored in base clases (see Vars...).
  ///
  ///@{
  /// Tile length
  num_t length;
  /// Tile level
  tree::level_idx level;

  ///@}  // Data

  void reinitialize(tree::level_idx level_, bbox_t bbox) noexcept {
    this->geometry().set_internal_bounding_box(bbox);
    length = this->geometry().tile_internal_length();
    level  = level;
    HM3_ASSERT(hg::approx(this->geometry().tile_internal_bounding_box(), bbox),
               "internal bounding box: {}, new bounding box: {}",
               this->geometry().tile_internal_bounding_box(), bbox);
    HM3_ASSERT(*level_ >= 0, "negative tile level {}", level_);
    HM3_ASSERT(length > 0., "zero tile length in tile with bbox: {}", bbox);

    call_as<Vars...>(*this, [](auto&& b) { b.reinitialize(); });

    // auto il = {(static_cast<void (tile::*)()>(&Vars::reinitialize))...};
    // for (auto&& i : il) this->*i();
  }

  void clear() {
    length = 0.;
    level  = level_idx{};
    call_as<Vars...>(*this, [](auto&& b) { b.clear(); });
  }

  tile() = default;
  tile(tree::level_idx level_, bbox_t bbox) {
    reinitialize(level_, std::move(bbox));
    clear();
  }
};

}  // namespace hm3::solver
