#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file File system utilities.
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <boost/filesystem.hpp>
#include <hom3/core/types.hpp>
#include <hom3/core/mpi.hpp>
// Options:
#define ENABLE_DBG_ 0
#include <hom3/core/dbg.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

namespace io {

/// File system utilities
namespace file {

/// Does file \p fileName exist?
bool exists(const String fileName, const boost::mpi::communicator& comm) {
  using namespace boost;
  return mpi::root_do(comm, [=]() { return filesystem::exists(fileName); });
}

/// Removes the file \p fileName _if_ it exists.
/// \returns True if file existed, false otherwise.
bool remove(const String fileName, const boost::mpi::communicator& comm) {
  using namespace boost;
  return mpi::root_do(comm, [=]() { return filesystem::remove(fileName); });
}

/// Remove file if exist
auto remove_if_exists(const String fName) {
  auto comm = boost::mpi::communicator();
  if (exists(fName, comm)) { remove(fName, comm); }
  return comm;
}

}  // namespace file

}  // namespace io
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
#undef ENABLE_DBG_
