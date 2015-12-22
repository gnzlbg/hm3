#pragma once
/// \file
///
/// Tree concepts
#include <array>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace tree {

namespace concepts {
namespace rc = ranges::concepts;

/// Locational codes model the location concept
struct location : rc::refines<rc::Regular, geometry::concepts::Dimensional,
                              rc::TotallyOrdered> {
  template <typename T>  //
  auto requires_(T&& t) -> decltype(
   rc::valid_expr((t.push(0_u), 42),  //
                  rc::convertible_to<uint_t>(t.pop()),
                  rc::convertible_to<level_idx>(t.max_level()),     //
                  rc::convertible_to<uint_t>(t.no_levels()),        //
                  rc::convertible_to<level_idx>(t.level()),         //
                  (t.reverse(), 42),                                //
                  rc::model_of<rc::RandomAccessRange>(t()),         //
                  rc::convertible_to<uint_t>(t[level_idx{}]),       //
                  rc::model_of<rc::RandomAccessRange>(t.levels()),  //
                  (T(std::initializer_list<uint_t>{0_u}), 42),      //
                  (T(std::vector<uint_t>{0_u, 0_u}), 42),           //
                  (compact_optional<T>{}, 42),                      //
                  rc::convertible_to<compact_optional<T>>(shift(
                   t, std::array<int_t, rc::uncvref_t<T>::dimension()>{})),
                  typename rc::uncvref_t<T>::integer_t{}));
};

using Location = location;

}  // namespace concepts

template <typename T>
using Location = concepts::rc::models<concepts::Location, ranges::uncvref_t<T>>;

template <typename Loc> using loc_int_t = typename Loc::integer_t;

}  // namespace tree
}  // namespace hm3
