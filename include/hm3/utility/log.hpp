#pragma once
/// \file
///
/// Logging utilities
#include <spdlog/spdlog.h>
#include <hm3/utility/config/assert.hpp>
#define HM3_LOG_TO_STDOUT
#define HM3_LOG_DEBUG

namespace hm3 {

namespace log {

using file_sink_t    = spdlog::sinks::simple_file_sink_st;
using console_sink_t = spdlog::sinks::stdout_sink_st;
using sink_ptr       = spdlog::sink_ptr;
using logger_ptr     = std::shared_ptr<spdlog::logger>;

inline sink_ptr file_sink() {
  static sink_ptr sink = std::make_shared<file_sink_t>("hm3.log", false);
  return sink;
}

inline sink_ptr console_sink() {
  static sink_ptr sink = console_sink_t::instance();
  return sink;
}

inline logger_ptr file_logger(string const& name) {
  auto fl = spdlog::details::registry::instance().get(name);
  if (fl) { return fl; }
  return spdlog::details::registry::instance().create(name, file_sink());
}

inline logger_ptr console_logger(string const& name) {
  string n = name + "'";
  auto fl  = spdlog::details::registry::instance().get(n);
  if (fl) { return fl; }
  return spdlog::details::registry::instance().create(n, console_sink());
}

namespace level {
using namespace spdlog::level;
}  // namespace level

/// A serial log
///
/// To output the logged data to the console define:  HM3_LOG_TO_STDOUT
/// To output debug logs by default define: HM3_LOG_DEBUG
///
/// Note: the logging functions have side-effects even tho they are const
/// They are not thread-safe but can be made thread safe by using "_mt" loggers
/// instead of "_st" loggers
struct serial {
 private:
  logger_ptr console_logger_;
  logger_ptr file_logger_;
  bool console_output_;

 public:
  serial()                    = default;
  serial(serial const& other) = default;
  serial(serial&&)            = default;
  serial& operator=(serial const&) = default;
  serial& operator=(serial&&) = default;

  serial(string const& name, bool enable_console = false)
   : console_logger_{console_logger(name)}
   , file_logger_{file_logger(name)}
   , console_output_{enable_console} {
#ifdef HM3_LOG_TO_STDOUT
    console_output_ = true;
#endif
#ifdef HM3_LOG_DEBUG
    level(::hm3::log::level::debug);
#endif
  }

  template <typename... Args> void operator()(Args&&... args) const {
    HM3_ASSERT(initialized(), "Trying to use an uninitialized log");
    if (console_output_) { console_logger_->info(args...); }
    file_logger_->info(std::forward<Args>(args)...);
  }

  bool initialized() const noexcept { return console_logger_ and file_logger_; }

  void console_output(bool value) { console_output_ = value; }
  bool console_output() const { return console_output_; }

  void level(level::level_enum value) {
    console_logger_->set_level(value);
    file_logger_->set_level(value);
  }

  template <typename... Args> void debug(Args&&... args) const {
    HM3_ASSERT(initialized(), "Trying to use an uninitialized log");
    if (console_output_) { console_logger_->debug(args...); }
    file_logger_->debug(std::forward<Args>(args)...);
  }

  template <typename... Args> void error(Args&&... args) const {
    HM3_ASSERT(initialized(), "Trying to use an uninitialized log");
    if (console_output_) { console_logger_->error(args...); }
    file_logger_->error(std::forward<Args>(args)...);
  }
};

struct console {
  serial& log_;
  console(serial& log) : log_(log) { log_.console_output(true); }
  ~console() { log_.console_output(false); }
  console(console&) = default;
  console& operator=(console&) = default;
  console(console&&)           = default;
  console& operator=(console&&) = default;
};

}  // namespace log
}  // namespace hm3

/// Logs any exceptions thrown at the interface between the plugin and paraview
#define HM3_LOG_EXCEPTIONS                                            \
  catch (const std::exception& e) {                                   \
    log.error("Exception thrown:\n\nmessage: \"{}\"\n\n{}", e.what(), \
              HM3_FMT_AT(HM3_AT_));                                   \
    throw e;                                                          \
  }
