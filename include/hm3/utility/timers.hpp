#pragma once
/// \file
///
/// Scope-based timers
///
/// Define the macro HM3_ENABLE_TIMERS to use them. They give a very borad
/// overview of where time within the program is spent.
#include <atomic>
#include <chrono>
#include <hm3/io/json.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>

/// Enables timers
#define HM3_ENABLE_TIMERS

namespace hm3 {

/// Performance timers
namespace timer {

namespace detail {

using clock_t      = std::chrono::high_resolution_clock;
using duration_t   = std::chrono::nanoseconds;
using time_point_t = std::chrono::time_point<clock_t, duration_t>;

string duration_units() { return "ns"; }

struct state;
struct updater;

/// Updates the timer of a node on destruction
struct updater {
  state* timer_;
  time_point_t start_;
  time_point_t end_;

  updater(updater const&) = delete;
  updater& operator=(updater const&) = delete;
  updater& operator=(updater&& other) = delete;

  string const& name() const noexcept;

  updater(state* n);
  updater(updater&& other);
  ~updater();
};

/// Timer state
struct state {
  /// Name of the timer.
  string name;
  /// Parent timer of the timer (if any).
  state* parent = nullptr;
  /// Is the timer running?
  /// \todo should have type std::atomic<bool>.
  bool running{false};
  /// Total running time of the timer's scope.
  duration_t time = duration_t();
  /// Number of times that the timer was updated.
  uint_t iterations = 0;
  /// Sub-timers (if any).
  std::map<string, state> sub_timers;

  state(string n) : name{std::move(n)} {
    HM3_ASSERT(name == "main", "only the 'main' timer can be constructed "
                               "without a parent (error at timer: {})",
               name);
  }

  state(string n, state* p) : name{std::move(n)}, parent(p) {}
  state(state const& other) = default;
  state(state&&)            = default;
  state& operator=(state const&) = default;
  state& operator=(state&&) = default;

  /// Start this timer
  updater start() { return {this}; }

  template <typename S, CONCEPT_REQUIRES_(ConvertibleTo<S, string>{})>
  void init_timer(S&& sub_timer) {
    if (sub_timers.count(sub_timer) == 0) {
      auto t = state(sub_timer, this);
      sub_timers.emplace(std::forward<S>(sub_timer), std::move(t));
    }
  }

  /// Start \p sub_timer .
  template <typename S, CONCEPT_REQUIRES_(ConvertibleTo<S, string>{})>
  updater start(S&& sub_timer) {
    init_timer(sub_timer);
    return sub_timers.at(std::forward<S>(sub_timer)).start();
  }

  /// Start \p sub_timer .
  template <typename S, typename... Ss,
            CONCEPT_REQUIRES_(ConvertibleTo<S, string>{}
                              and meta::and_<ConvertibleTo<Ss, string>...>{})>
  updater start(S&& sub_timer, Ss&&... ss) {
    init_timer(sub_timer);
    return sub_timers.at(std::forward<S>(sub_timer))
     .start(std::forward<Ss>(ss)...);
  }

  /// Is any subtimer of this timer running?
  state const* any_subtimer_running() const noexcept {
    for (auto&& st : sub_timers | view::values) {
      if (st.running) { return &st; }
      if (auto* result = st.any_subtimer_running()) { return result; }
    }
    return nullptr;
  }
};

namespace to_json {

io::json gather(state const& t) {
  io::json data;

  auto clear_function_name = [](auto const& s) {
    auto last_semicolon = s.find_last_of(':');
    if (last_semicolon == std::string::npos) { return s; }
    auto first_parent = s.find_first_of('(', last_semicolon);
    return s.substr(last_semicolon + 1, first_parent - (last_semicolon + 1));
  };

  data["name"] = clear_function_name(t.name);
  data["total_running_time"]
   = std::chrono::duration_cast<std::chrono::seconds>(t.time).count();
  data["no_iterations"] = t.iterations;
  if (t.iterations > 0) {
    data["time_per_iteration"] = t.time.count() / t.iterations;
  }
  if (t.parent) { data["parent"] = clear_function_name(t.parent->name); }
  data["is_running"]             = t.running;

  if (t.parent == nullptr) {
    data["total_percent"] = 100.00;
  } else {
    auto* p = &t;
    while (p->parent != nullptr) { p = p->parent; }
    // p points to main:
    data["total_percent"]
     = p->time.count() != 0 ? (num_t)(t.time.count()) / (num_t)(p->time.count())
                            : math::inf;
  }
  if (t.parent == nullptr) {
    data["parent_percent"] = 100.00;
  } else {
    data["parent_percent"]
     = t.parent->time.count() != 0
        ? (num_t)(t.time.count()) / (num_t)(t.parent->time.count())
        : math::inf;
  }

  auto level = 0;
  auto* p    = t.parent;
  while (p != nullptr) {
    ++level;
    p = p->parent;
  }
  data["level"] = level;

  for (auto const& sub_timer : t.sub_timers | view::values) {
    data["children"].push_back(gather(sub_timer));
  }
  return data;
}

template <typename F>
void top_down(io::json const& d, F&& f) {
  f(d);
  if (io::has_field(d, "children")) {
    for (auto&& c : d["children"]) { top_down(c, f); }
  }
}

std::string format(io::json const& data) {
  static constexpr std::size_t indentation_per_level = 4;

  // compute max indentation level and max name length:
  io::json tmp           = data;
  auto max_level         = 0;
  auto name_max_l        = 0;
  auto total_perc_max_l  = 0;
  auto parent_perc_max_l = 0;
  auto time_max_l        = 0;
  auto time_it_max_l     = 0;

  top_down(data, [&](auto&& v) {
    int level = v["level"];

    string const& name = v["name"];
    int name_l         = name.size();

    int total_perc_l  = size(fmt::format("{}", num_t(v["total_percent"])));
    int parent_perc_l = size(fmt::format("{}", num_t(v["parent_percent"])));

    int time_l    = size(fmt::format("{:.2f}", num_t(v["total_running_time"])));
    int time_it_l = size(fmt::format("{:.0f}", num_t(v["time_per_iteration"])));

    max_level = std::max(level, max_level);

    name_max_l = std::max(name_max_l, name_l);

    total_perc_max_l  = std::max(total_perc_max_l, total_perc_l);
    parent_perc_max_l = std::max(parent_perc_max_l, parent_perc_l);

    time_max_l    = std::max(time_max_l, time_l);
    time_it_max_l = std::max(time_it_max_l, time_it_l);
  });

  auto name_l = indentation_per_level * max_level + name_max_l;

  time_max_l    = std::max(time_max_l, 9);
  time_it_max_l = std::max(time_it_max_l, 10);

  fmt::MemoryWriter o;

  o.write(" _{0:_^{1}}___{0:_^9}___{0:_^10}___{0:_^{2}}___{0:_^{3}}_\n", "_",
          name_l, time_max_l, time_it_max_l);
  o.write("| {0:^{1}} | {0:^9} | {0:^10} | {0:^{2}} | {0:^{3}} |\n", " ",
          name_l, time_max_l, time_it_max_l);
  o.write("| {0:^{1}} | {2:^9} | {3:^10} | {4:^{5}} | {6:^{7}} |\n", "name",
          name_l, "total [%]", "parent [%]", "total [s]", time_max_l,
          "t/it [ns]", time_it_max_l);
  o.write("|_{0:_^{1}}_|_{0:_^9}_|_{0:_^10}_|_{0:_^{2}}_|_{0:_^{3}}_|\n", "_",
          name_l, time_max_l, time_it_max_l);

  top_down(data, [&](auto&& v) {
    string name = string((int)v["level"] * indentation_per_level, ' ')
                  + (string const&)v["name"];
    o.write("| {0:<{1}} | {2:9.2f} | {3:10.2f} | {4:{5}.2f} | {6:{7}.0f} |\n",
            name, name_l, (num_t)v["total_percent"], (num_t)v["parent_percent"],
            (num_t)v["total_running_time"], time_max_l,
            (num_t)v["time_per_iteration"], time_it_max_l);
  });
  o.write("|_{0:_^{1}}_|_{0:_^9}_|_{0:_^10}_|_{0:_^{2}}_|_{0:_^{3}}_|\n", "_",
          name_l, time_max_l, time_it_max_l);

  return o.str();
}

}  // namespace to_json

/// Timer registry
struct registry {
  state main_{"main"};
  std::unique_ptr<updater> main_updater_;

  io::json data() const { return to_json::gather(main_); }

  // The timer updater for main is put on the heap so that we can safely destroy
  // it (and stop and commit the changes)
  registry() : main_updater_(std::make_unique<updater>(main_.start())) {}

  ~registry() {
    main_updater_.reset();  // The timer updater for main is destroyed before

    // outputing the results (to stop the main timer)
    auto fd = to_json::format(data());
    fmt::print("{}\n", fd);
  }

  template <typename S, typename... Ss,
            CONCEPT_REQUIRES_(ConvertibleTo<S, string>{}
                              and meta::and_<ConvertibleTo<Ss, string>...>{})>
  auto start(S&& s, Ss&&... ss) {
    return main_.start(std::forward<S>(s), std::forward<Ss>(ss)...);
  }
};

/// Returns the timer registry
registry& initialized() {
  static registry timers;
  return timers;
}

updater::updater(state* n) : timer_(n) {
  HM3_ASSERT(timer_, "cannot construct updater without a valid timer!");
  // TODO: bool result = timer_->running.exchange(true);
  bool result     = timer_->running;
  timer_->running = true;
  HM3_ASSERT(!result, "timer {} is already started!", timer_->name);
  HM3_ASSERT(
   [&]() {
     auto* cr = timer_->any_subtimer_running();
     HM3_ASSERT(cr == nullptr, "child timer {} of {} is already running!",
                cr->name, timer_->name);
     return true;
   }(),
   "");
  HM3_ASSERT((timer_->parent and timer_->parent->running)
              || (!timer_->parent and timer_->name == "main"),
             "parent timer {} of timer {} is not running! Timers:\n\n{}",
             timer_->parent->name, timer_->name, initialized().data().dump(4));

  start_ = clock_t::now();
}
updater::~updater() {
  if (!timer_) { return; }
  end_ = clock_t::now();
  HM3_ASSERT(end_ >= start_, "start time {}{} !<= end time {}{}",
             start_.time_since_epoch().count(), duration_units(),
             end_.time_since_epoch().count(), duration_units());
  timer_->time += end_ - start_;
  timer_->iterations += 1;

  HM3_ASSERT(
   [&]() {
     auto* cr = timer_->any_subtimer_running();
     HM3_ASSERT(cr == nullptr, "child timer {} of {} is still running!",
                cr->name, timer_->name);
     return true;
   }(),
   "");

  // TODO: auto result = timer_->running.exchange(false);
  auto result     = timer_->running;
  timer_->running = false;
  HM3_ASSERT(result, "timer {} was already stopped!", timer_->name);

  HM3_ASSERT((timer_->parent and timer_->parent->running)
              || (!timer_->parent and timer_->name == "main"),
             "parent timer {} of timer {} is not running! Timers:\n\n{}",
             timer_->parent->name, timer_->name, initialized().data().dump(4));
}

updater::updater(updater&& other)
 : timer_(other.timer_), start_(other.start_), end_(other.end_) {
  HM3_ASSERT(other.timer_, "move construction from invalid timer {}",
             other.timer_->name);
  other.timer_ = nullptr;
}

string const& updater::name() const noexcept { return timer_->name; }

}  // namespace detail

/// Initialize timers
void initialize() { detail::initialized(); }

/// Start timer
template <typename S, typename... Ss,
          CONCEPT_REQUIRES_(ConvertibleTo<S, string>{}
                            and meta::and_<ConvertibleTo<Ss, string>...>{})>
auto start(S&& s, Ss&&... ss) {
  return detail::initialized().start(std::forward<S>(s),
                                     std::forward<Ss>(ss)...);
}

}  // namespace timer
}  // namespace hm3

#ifdef HM3_ENABLE_TIMERS

#define HM3_TIMER_VAR1(X) hm3_scope_timer_at_##X
#define HM3_TIMER_VAR0(X) HM3_TIMER_VAR1(X)
#define HM3_TIMER_VAR() HM3_TIMER_VAR0(__LINE__)

/// Initializes all timers
#define HM3_INITIALIZE_TIMERS() ::hm3::timer::initialize()

/// Time current scope
#define HM3_TIME(...) \
  auto HM3_TIMER_VAR() { ::hm3::timer::start(__VA_ARGS__) }

/// Time type
#define HM3_TIME_TYPE(name) \
  hm3::timer::detail::updater hm3_type_timer { ::hm3::timer::start(#name) }

/// Time member function
#define HM3_TIME_MEMF() \
  ::hm3::timer::start(hm3_type_timer.name(), __PRETTY_FUNCTION__)

#else

#define HM3_INITIALIZE_TIMERS()
#define HM3_TIME(...)
#define HM3_TIME_TYPE(name)
#define HM3_TIME_MEMF()

#endif
