#pragma once
/// \file sort.hpp
#include <utility>

namespace hm3 {
namespace solver {

template <typename Solver> void sort(Solver&& s) { s.sort(); }

template <typename Solver, typename... Solvers>
void sort(Solver&& s, Solvers&&... ss) {
  s.sort();
  sort(std::forward<Solvers>(ss)...);
}

}  // namespace solver
}  // namespace hm3
