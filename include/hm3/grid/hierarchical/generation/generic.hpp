#pragma once
/// \file
///
/// Generic grid generator
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::grid::hierarchical::generation {

struct generic_fn {
  template <typename Range, typename Refine, typename Predicate,
            typename AfterIteration>
  auto operator()(Range&& rng, Refine&& refine, Predicate&& predicate,
                  AfterIteration&& after_iteration) const noexcept {
    bool done = false;
    while (!done) {
      done = true;
      for (auto&& i : rng()) {
        if (predicate(i)) {
          done = false;
          refine(i);
        }
      }
      after_iteration();
    }
  }
};

namespace {
auto&& generic = static_const<generic_fn>::value;
}  // namespace

}  // namespace hm3::grid::hierarchical::generation
