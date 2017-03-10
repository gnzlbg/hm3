#pragma once
/// \file
///
/// File system utilities.
#include <fstream>
#include <hm3/config/system.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/mpi.hpp>
#include <hm3/utility/tuple.hpp>
#include <streambuf>
// Filesystem headers:
#if defined(_POSIX_VERSION) || defined(__APPLE__)
#include <sys/stat.h>
#include <cstdio>
#else
#pragma error "error: system not supported"
#endif

namespace hm3 {

/// File system utilities
namespace fs {

/// Does file \p file_path exist?
CONCEPT_REQUIRES(std::is_convertible<system::tag_t, system::posix>{})
bool exists_(string const& file_path) {
  struct stat buffer;
  return (stat(file_path.c_str(), &buffer) == 0);
}

/// Does file \p file_path exist?
CONCEPT_REQUIRES(std::is_convertible<system::tag_t, system::posix>{})
bool exists(string const& file_path, const mpi::comm comm) {
  return mpi::root_do(comm, [&]() { return exists_(file_path); });
}

/// Removes the file \p file_path _if_ it exists.
/// \returns True if file existed, false otherwise.
CONCEPT_REQUIRES(std::is_convertible<system::tag_t, system::posix>{})
bool remove_(string const& file_path, const mpi::comm& comm) {
  if (exists(file_path, comm)) { return std::remove(file_path.c_str()) == 0; }
  return false;
}

/// Removes the file \p file_path _if_ it exists.
/// \returns True if file existed, false otherwise.
CONCEPT_REQUIRES(std::is_convertible<system::tag_t, system::posix>{})
bool remove(string const& file_path, const mpi::comm& comm) {
  return mpi::root_do(comm, [&]() { return remove_(file_path, comm); });
}

/// Splits extension from file path -> (path_without_extension, extension)
inline std::pair<string, string> split_extension(string const& file_path) {
  const auto last_period = file_path.find_last_of('.');
  if (last_period == string::npos) {
    return std::make_pair(file_path, string{});
  }
  return std::make_pair(file_path.substr(0, last_period),
                        file_path.substr(last_period + 1));
}

/// Splits directory from the file path -> (directory, file_name)
inline std::pair<string, string> split_dir(string const& file_path) {
  const auto last_slash = file_path.find_last_of('/');
  if (last_slash == string::npos) {
    return std::make_pair(string{}, file_path);
  }
  return std::make_pair(file_path.substr(0, last_slash),
                        file_path.substr(last_slash + 1));
}

/// Splits file path -> (directory, file_base_name, extension)
inline tuple<string, string, string> split_path(string const& file_path) {
  string dir, base_name, ext;
  std::tie(dir, base_name) = split_dir(file_path);
  std::tie(base_name, ext) = split_extension(base_name);
  return std::make_tuple(dir, base_name, ext);
}

inline string make_file_name(string const& base_name, string const& extension) {
  if (extension.empty()) { return base_name; }
  return base_name + '.' + extension;
}

inline string make_file_path(string const& dir, string const& file_name) {
  if (dir.empty()) { return file_name; }
  return dir + '/' + file_name;
}

inline string make_file_path(string const& dir, string const& file_name,
                             string const& extension) {
  return make_file_path(dir, make_file_name(file_name, extension));
}

inline string name(string const& file_name) {
  return split_extension(file_name).first;
}
inline string extension(string const& file_name) {
  return split_extension(file_name).second;
}

inline string read_whole(string const& file_name, const mpi::comm& comm) {
  if (!exists(file_name, comm)) {
    HM3_FATAL_ERROR("file name \"{}\" does not exist!", file_name);
  }
  std::ifstream t(file_name);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  return str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
}

/// Returns true if both files are bitwise identical
inline bool binary_identical(string const& file_name0, string const& file_name1,
                             mpi::comm const& comm) {
  auto f0 = read_whole(file_name0, comm);
  auto f1 = read_whole(file_name1, comm);
  return f0 == f1;
}

}  // namespace fs
}  // namespace hm3
