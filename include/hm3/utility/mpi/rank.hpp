#pragma once
/// \file
///
/// MPI Rank
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {
namespace mpi {

using rank_t
 = compact_optional<empty_scalar_value<int, std::numeric_limits<int>::min()>,
                    struct mpi_rank_tag>;

static constexpr auto root = rank_t{0};

using tag = int;

}  // namespace mpi
}  // namespace hm3
