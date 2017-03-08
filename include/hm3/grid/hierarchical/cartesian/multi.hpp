#pragma once
/// \file
///
/// Multiple hierarchical Cartesian Grids
#include <hm3/grid/hierarchical/adaptor/multi.hpp>
#include <hm3/grid/hierarchical/adaptor/serialization/multi_fio.hpp>
#include <hm3/grid/hierarchical/cartesian/serialization/single_fio.hpp>
#include <hm3/grid/hierarchical/cartesian/single.hpp>

namespace hm3::grid::hierarchical::cartesian {

/// Multiple hierarchical Cartesian Grids
///
///
template <dim_t Ad>
struct multi : adaptor::multi<single<Ad>> {
  using base_t = adaptor::multi<single<Ad>>;
  io::client io_;
  hm3::log::serial log;

  multi() = default;

  multi(io::session& s, base_t&& m)
   : base_t(std::move(m)), io_(s, name(m), type(m)), log(name(m)) {
    log("Initialized:\n"
        "  - max_no_nodes: {}\n"
        "  - no_grids: {}\n"
        "  - bounding_box: {}\n"
        "  - no_nodes: {}\n",
        this->capacity(), this->no_grids(), this->bounding_box(), this->size());
  }
  multi(io::session& s, tree_node_idx node_capacity, grid_idx grid_capacity,
        geometry::box<Ad> bounding_box)
   : multi(s, base_t(node_capacity, grid_capacity, std::move(bounding_box))) {}

  bool is_sorted() const noexcept { return ::hm3::tree::dfs_sort.is(*this); }

  auto write() {
    HM3_ASSERT(is_sorted(), "cannot write unsorted grid");
    auto f = io_.new_file();
    to_file_unwritten(f, static_cast<base_t const&>(*this));
    io_.write(f);
  }

  static multi<Ad> from_session(io::session& s, string const& type_,
                                string const& name_,
                                io::file::index_t i = io::file::index_t{},
                                tree_node_idx node_capacity = tree_node_idx{},
                                grid_idx grid_capacity      = grid_idx{}) {
    io::client c(s, name_, type_);
    auto f = c.get_file(i);

    if (type_ == type(base_t{})) {
      auto d = from_file(base_t{}, f, node_capacity, grid_capacity);
      return multi<Ad>{s, std::move(d)};
    }

    if (type_ == type(single<Ad>{})) {
      auto d = from_file(single<Ad>{}, f, node_capacity);
      return multi<Ad>{s, base_t{0, std::move(d)}};
    }

    if (type_ == type(tree::tree<Ad>{})) {
      auto d = from_file(tree::tree<Ad>{}, f, node_capacity);
      return multi<Ad>{
       s, base_t{0, single<Ad>{geometry::box<Ad>::unit(), std::move(d)}}};
    }

    HM3_FATAL_ERROR("Cannot read block of type {} and name {} into a "
                    "grid::hierarchical::cartesian::multi",
                    type_, name_);
  }
};

template <dim_t Ad>
string name(multi<Ad>) {
  return name(typename multi<Ad>::base_t{});
}

template <dim_t Ad>
string type(multi<Ad>) {
  return type(typename multi<Ad>::base_t{});
}

template <dim_t Ad>
bool operator==(multi<Ad> const& a, multi<Ad> const& b) noexcept {
  using base_t = typename multi<Ad>::base_t const&;
  return static_cast<base_t>(a) == static_cast<base_t>(b);
}

template <dim_t Ad>
bool operator!=(multi<Ad> const& a, multi<Ad> const& b) noexcept {
  return !(a == b);
}

}  // namespace hm3::grid::hierarchical::cartesian
