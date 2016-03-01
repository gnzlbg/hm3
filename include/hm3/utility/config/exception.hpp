#pragma once
#include <hm3/utility/config/at.hpp>

/// Reports any exceptions thrown to stderr at the interface between the plugin
/// and paraview (usefull when the log might be dead:
/// constructors/destructors/static function/...)
#define HM3_REPORT_EXCEPTIONS                                             \
  catch (const std::exception& e) {                                       \
    ::hm3::fmt::print(stderr, "Exception thrown: {}\n\n{}\n\n", e.what(), \
                      HM3_FMT_AT(HM3_AT_));                               \
    throw e;                                                              \
  }
