#ifdef FIXED
#pragma once
/// \file
///
///
namespace hm3 {
namespace solver {
namespace fv {

/// Tag for conservative variables
struct cv_t {};
static constexpr cv_t cv{};

/// Tag for primitive variables
struct pv_t {};
static constexpr pv_t pv{};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
#endif
