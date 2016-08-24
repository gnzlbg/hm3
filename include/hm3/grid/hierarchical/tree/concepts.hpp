#pragma once
/// \file
///
/// Tree concepts
#include <hm3/geometry/dimension.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace tree {

template <typename Loc>
using morton_idx_t = typename Loc::morton_idx_t;
template <typename Loc>
using morton_x_t = typename Loc::morton_x_t;

namespace concepts {
namespace rc = ranges::concepts;

/// Locational codes model the location concept
struct location : rc::refines<rc::Regular, geometry::concepts::Dimensional,
                              rc::TotallyOrdered> {
  template <typename T, typename IdxT = morton_idx_t<uncvref_t<T>>,
            typename XT = morton_x_t<uncvref_t<T>>>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr((t.push(cpidx_t{0}), 42),  //
                  rc::convertible_to<cpidx_t>(t.pop()),
                  rc::convertible_to<level_idx>(t.max_level()),         //
                  rc::convertible_to<lidx_t>(t.max_no_levels()),        //
                  rc::convertible_to<level_idx>(t.level()),             //
                  rc::convertible_to<bool>(t.valid()),                  //
                  rc::convertible_to<IdxT>(t.morton_idx()),             //
                  rc::convertible_to<XT>(t.morton_x()),                 //
                  (t.reverse(), 42),                                    //
                  rc::model_of<rc::RandomAccessRange>(t()),             //
                  (static_cast<IdxT>(t)),                               //
                  (static_cast<XT>(t)),                                 //
                  rc::convertible_to<cpidx_t>(t[level_idx{}]),          //
                  rc::model_of<rc::RandomAccessRange>(t.levels()),      //
                  (T(std::initializer_list<cpidx_t>{cpidx_t{0}}), 42),  //
                  (T(vector<cpidx_t>{cpidx_t{0}, cpidx_t{0}}), 42),     //
                  (compact_optional<T>{}, 42),                          //
                  rc::convertible_to<compact_optional<T>>(
                   shift(t, offset_t<rc::uncvref_t<T>::dimension()>{}))  //
                  ));
};

using Location = location;

}  // namespace concepts

template <typename T>
using Location = concepts::rc::models<concepts::Location, ranges::uncvref_t<T>>;

}  // namespace tree
}  // namespace hm3
