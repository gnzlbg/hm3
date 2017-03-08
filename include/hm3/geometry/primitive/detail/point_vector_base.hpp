#pragma once
/// \file
///
/// Point/vector base
#include <hm3/utility/matrix.hpp>

namespace hm3::geometry {

/// Implementation detail of point and vec primitive types.
namespace point_vector_detail {

template <dim_t Ad>
using base_t = dense::vector<num_t, static_cast<int_t>(Ad), dim_t>;

}  // namespace point_vector_detail
}  // namespace hm3::geometry
