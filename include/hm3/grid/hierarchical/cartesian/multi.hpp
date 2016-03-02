#pragma once
/// \file
///
/// Multiple hierarchical Cartesian Grids
#include <hm3/grid/hierarchical/adaptor/multi.hpp>
#include <hm3/grid/hierarchical/adaptor/serialization/multi_fio.hpp>
#include <hm3/grid/hierarchical/cartesian/serialization/single_fio.hpp>
#include <hm3/grid/hierarchical/cartesian/single.hpp>

namespace hm3 {
namespace grid {

namespace hc {

/// Multiple hierarchical Cartesian Grids
///
///
template <uint_t Nd> struct multi : adaptor::multi<single<Nd>> {
  using base_t = adaptor::multi<single<Nd>>;
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
        geometry::square<Nd> bounding_box)
   : multi(s, base_t(node_capacity, grid_capacity, std::move(bounding_box))) {}

  bool is_sorted() const noexcept { return ::hm3::tree::dfs_sort.is(*this); }

  auto write() {
    HM3_ASSERT(is_sorted(), "cannot write unsorted grid");
    auto f = io_.new_file();
    to_file_unwritten(f, static_cast<base_t const&>(*this));
    io_.write(f);
  }

  static multi<Nd> from_session(io::session& s, string const& type_,
                                string const& name_,
                                io::file::index_t i = io::file::index_t{},
                                tree_node_idx node_capacity = tree_node_idx{},
                                grid_idx grid_capacity      = grid_idx{}) {
    io::client c(s, name_, type_);
    auto f = c.get_file(i);

    if (type_ == type(base_t{})) {
      auto d = from_file(base_t{}, f, node_capacity, grid_capacity);
      return multi<Nd>{s, std::move(d)};
    }

    if (type_ == type(single<Nd>{})) {
      auto d = from_file(single<Nd>{}, f, node_capacity);
      return multi<Nd>{s, base_t{0, std::move(d)}};
    }

    if (type_ == type(tree::tree<Nd>{})) {
      auto d = from_file(tree::tree<Nd>{}, f, node_capacity);
      return multi<Nd>{
       s, base_t{0, single<Nd>{geometry::unit(geometry::square<Nd>{}),
                               std::move(d)}}};
    }

    HM3_FATAL_ERROR(
     "Cannot read block of type {} and name {} into a grid::hc::multi", type_,
     name_);
  }
};

template <uint_t Nd> string name(multi<Nd>) {
  return name(typename multi<Nd>::base_t{});
}

template <uint_t Nd> string type(multi<Nd>) {
  return type(typename multi<Nd>::base_t{});
}

template <uint_t Nd>
bool operator==(multi<Nd> const& a, multi<Nd> const& b) noexcept {
  using base_t = typename multi<Nd>::base_t const&;
  return static_cast<base_t>(a) == static_cast<base_t>(b);
}

template <uint_t Nd>
bool operator!=(multi<Nd> const& a, multi<Nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace hc
}  // namespace grid
}  // namespace hm3
