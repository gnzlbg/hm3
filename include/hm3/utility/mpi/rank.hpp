#pragma once
/// \file
///
/// MPI Rank
#include <hm3/utility/optional_idx.hpp>

namespace hm3::mpi {

// TODO: why lowest?
using rank_t = optional_idx<int, struct mpi_rank_tag, math::lowest<int>>;

static constexpr auto root = rank_t{0};

namespace literals {

rank_t operator""_r(unsigned long long idx) { return rank_t{idx}; }

}  // namespace literals

}  // namespace hm3::mpi
