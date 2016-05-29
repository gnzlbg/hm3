#pragma once
/// \file
///
/// Includes the json library and adds some utilities for HM3's File I/O
#include <hm3/types.hpp>
#include <hm3/utility/config/exception.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <json.hpp>

namespace hm3 {
namespace io {

using namespace nlohmann;

inline bool has_field(json const& j, string const& name) {
  return j.count(name) != 0u;
}

template <typename At>
inline void require_field(json const& j, string const& name, At&& at) {
  if (!has_field(j, name)) {
    HM3_FATAL_ERROR_AT(at, "field \"{}\" is required by json entry:\n\n{}",
                       name, j.dump());
  }
}

template <typename At>
inline auto read_field(json const& j, string const& name, At&& at) {
  require_field(j, name, std::forward<At>(at));
  return j[name];
}

template <typename At>
inline auto read_file_field(io::json const& j, string const& name, At&& at) {
  require_field(j["files"][0]["fields"], name, std::forward<At>(at));
  return j["files"][0]["fields"][name];
}

}  // namespace io
}  // namespace hm3
