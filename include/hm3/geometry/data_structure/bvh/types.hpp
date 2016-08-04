#pragma once
/// \file
///
/// Bounding Volume Hierarchy types
#include <hm3/types.hpp>
#include <hm3/utility/optional_idx.hpp>

namespace hm3::geometry::bvh {

/// BVH Node index (weakly-typed)
using idx_t = suint_t;

/// BVH simplex index (weakly-typed)
using sidx_t = suint_t;

/// Index of a BVH node within a BVH
using node_idx = optional_idx<idx_t, struct node_idx_tag>;

/// Index of a simplex within a BVH
using simplex_idx = optional_idx<sidx_t, struct simplex_idx_tag>;

}  // namespace hm3::geometry::bvh
