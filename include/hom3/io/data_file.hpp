#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Data file
#define HDF5_DATA_FILE
////////////////////////////////////////////////////////////////////////////////
// Include:
#include <hom3/io/access_modes.hpp>
#include <hom3/io/error.hpp>
#include <hom3/io/layout.hpp>
#ifdef HDF5_DATA_FILE
#include <hom3/io/hdf5_data_file.hpp>
#endif
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {
namespace io {

/// I/O utilities for data
namespace data {

/// Default data file
template <class AccessMode> using file = hdf5_file<AccessMode>;

using file_w = file<access_mode::write>;
using file_r = file<access_mode::read>;

template <template <class> class data_file> String extension() {
  return data_file<access_mode::read>::extension();
}

}  // namespace data
}  // namespace io
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
