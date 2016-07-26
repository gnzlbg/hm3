#pragma once
/// \file
///
/// MPI Rank
#include <hm3/utility/optional_idx.hpp>

namespace hm3 {
namespace mpi {

// TODO: why lowest?
using rank_t = optional_idx<int, struct mpi_rank_tag, math::lowest<int>>;

static constexpr auto root = rank_t{0};

using tag = int;

}  // namespace mpi
}  // namespace hm3
