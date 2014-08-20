#pragma once
/// \file multi_hc.hpp Multiple hierarchical Cartesian Grids
#include <hm3/grid/hc/single.hpp>
#include <hm3/grid/hc/serialization/single_fio.hpp>
#include <hm3/grid/adaptor/multi.hpp>
#include <hm3/grid/adaptor/serialization/multi/fio.hpp>

namespace hm3 {
namespace grid {

namespace hc {

/// Multiple hierarchical Cartesian Grids
///
///
template <uint_t nd> struct multi : adaptor::multi<single<nd>> {
  using base_t = adaptor::multi<single<nd>>;
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
        geometry::square<nd> bounding_box)
   : multi(s, base_t(node_capacity, grid_capacity, std::move(bounding_box))) {}

  bool is_sorted() const noexcept { return ::hm3::tree::dfs_sort.is(*this); }

  auto write() {
    HM3_ASSERT(is_sorted(), "cannot write unsorted grid");
    auto f = io_.new_file();
    to_file_unwritten(f, static_cast<base_t const&>(*this));
    io_.write(f);
  }

  static multi<nd> from_session(io::session& s, string const& type_,
                                string const& name_,
                                io::file::index_t i = io::file::index_t{},
                                tree_node_idx node_capacity = tree_node_idx{},
                                grid_idx grid_capacity = grid_idx{}) {
    io::client c(s, name_, type_);
    auto f = c.get_file(i);

    if (type_ == type(base_t{})) {
      auto d = from_file(base_t{}, f, node_capacity, grid_capacity);
      return multi<nd>{s, std::move(d)};
    }

    if (type_ == type(single<nd>{})) {
      auto d = from_file(single<nd>{}, f, node_capacity);
      return multi<nd>{s, base_t{0, std::move(d)}};
    }

    if (type_ == type(tree::tree<nd>{})) {
      auto d = from_file(tree::tree<nd>{}, f, node_capacity);
      return multi<nd>{
       s, base_t{0, single<nd>{geometry::square<nd>::unit(), std::move(d)}}};
    }

    HM3_TERMINATE(
     "Cannot read block of type {} and name {} into a grid::hc::multi", type_,
     name_);
  }
};

template <uint_t nd> string name(multi<nd>) {
  return name(typename multi<nd>::base_t{});
}

template <uint_t nd> string type(multi<nd>) {
  return type(typename multi<nd>::base_t{});
}

template <uint_t nd>
bool operator==(multi<nd> const& a, multi<nd> const& b) noexcept {
  using base_t = typename multi<nd>::base_t const&;
  return static_cast<base_t>(a) == static_cast<base_t>(b);
}

template <uint_t nd>
bool operator!=(multi<nd> const& a, multi<nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace hc
}  // namespace grid
}  // namespace hm3
