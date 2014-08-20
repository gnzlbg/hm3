#pragma once
/// \file
///
/// Writes a group of solvers in order
#include <utility>
namespace hm3 {
namespace solver {

template <typename Solver> void write(Solver&& s) { s.write(); }

template <typename Solver, typename... Solvers>
void write(Solver&& s, Solvers&&... ss) {
  s.write();
  write(std::forward<Solvers>(ss)...);
}

}  // namespace solver
}  // namespace hm3
