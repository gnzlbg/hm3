#pragma once
/// \file
///
///
/*
namespace hm3 {
namespace solver {

struct time_stepper {
  num_t time_end = 0.;
  num_t time     = 0.;
  num_t dt       = 0.;
  uint_t step    = 0;

  template <typename... Num, CONCEPT_REQUIRES_(Same<Num, num_t>{})>
  num_t set_time_step(Num&&... dts) noexcept {
    num_t min_dt = min(std::forward<Num>(dts)...);
    dt = min_dt;
    return min_dt;
  }

  void advance() noexcept {
    if (*this) {
      time += dt;
    } else {
      time = time_end;
    }
    ++step;
  }

  explicit operator bool() const noexcept { return time + dt < time_end; }
};

}  // namespace solver
}  // namespace hm3
*/
