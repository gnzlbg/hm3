#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file MPI utilities (Message Passing Interface)
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <boost/mpi.hpp>
#include <type_traits>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

/// MPI utilities (Message Passing Interface)
namespace mpi {

using namespace boost::mpi;

inline MPI_Info info() noexcept { return MPI_INFO_NULL; }

template <std::size_t N> using requests = std::array<boost::mpi::request, N>;

template <std::size_t N> auto wait_all(requests<N>& reqs) {
  return wait_all(std::begin(reqs), std::end(reqs));
}

/// Is rank the root rank?
bool is_root_rank(int rank) { return rank == 0; }
/// Is this process the root process of the communicator?
bool is_root(const mpi::communicator& comm) {
  return is_root_rank(comm.rank());
}

namespace detail {

struct not_void {};
struct is_void {};
template <class T> struct is_void_type { using type = not_void; };
template <> struct is_void_type<void> { using type = is_void; };

template <class F, class... Args>
constexpr void root_do_(is_void, const communicator& comm, F&& f,
                        Args&&... args) noexcept {
  if (is_root(comm)) { f(std::forward<Args>(args)...); }
  return;
}

template <class F, class... Args>
inline constexpr auto root_do_(
 not_void, const communicator& comm, F&& f,
 Args&&... args) noexcept -> decltype(f(std::forward<Args>(args)...)) {
  decltype(f(std::forward<Args>(args)...)) result{};
  if (is_root(comm)) { result = f(std::forward<Args>(args)...); }
  boost::mpi::broadcast(comm, result, 0);
  return result;
}

}  // namespace detail

/// Performs a computation _only_ at the root of the communicator
///
/// If the computation has a return type, this type is broadcasted to all
/// MPI Processes.
template <class F, class... Args>
inline constexpr auto root_do(const communicator& comm, F&& f, Args&&... args)
 -> decltype(f(std::forward<Args>(args)...)) {
  using is_void = typename detail::is_void_type<decltype(
   f(std::forward<Args>(args)...))>::type;
  return detail::root_do_(is_void(), comm, std::forward<F>(f),
                          std::forward<Args>(args)...);
}

}  // namespace mpi

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
