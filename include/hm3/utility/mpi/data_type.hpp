#pragma once
/// \file
///
/// Conversion from C++ types to MPI Datatypes
#include <hm3/utility/mpi/config.hpp>

namespace hm3::mpi {

/// Integers:
inline auto data_type(int8_t) { return MPI_INT8_T; }
inline auto data_type(int16_t) { return MPI_INT16_T; }
inline auto data_type(int32_t) { return MPI_INT32_T; }
inline auto data_type(int64_t) { return MPI_INT64_T; }
inline auto data_type(uint8_t) { return MPI_UINT8_T; }
inline auto data_type(uint16_t) { return MPI_UINT16_T; }
inline auto data_type(uint32_t) { return MPI_UINT32_T; }
inline auto data_type(uint64_t) { return MPI_UINT64_T; }

/// Bool
inline auto data_type(bool) { return MPI_CXX_BOOL; }

/// Floating point:
inline auto data_type(float) { return MPI_FLOAT; }
inline auto data_type(double) { return MPI_DOUBLE; }
inline auto data_type(long double) { return MPI_LONG_DOUBLE; }

/// Chars:
inline auto data_type(char) { return MPI_CHAR; }
inline auto data_type(wchar_t) { return MPI_WCHAR; }

}  // namespace hm3::mpi
