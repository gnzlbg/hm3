#pragma once
/// \file
///
/// MPI communicator
#include <hm3/utility/fmt.hpp>
#include <hm3/utility/mpi/config.hpp>
#include <hm3/utility/mpi/data_type.hpp>
#include <hm3/utility/mpi/rank.hpp>
#include <hm3/utility/mpi/call.hpp>
#include <hm3/utility/mpi/functions.hpp>

namespace hm3 {
namespace mpi {

/// MPI Communicator
struct comm {
 private:
  MPI_Comm comm_id_;

 public:
  comm(MPI_Comm c = MPI_COMM_NULL) : comm_id_(c) {}

  comm(comm const& other) = default;
  comm& operator=(comm const& other) = default;

  comm(comm&&)  = default;
  comm& operator=(comm&&) = default;

  bool empty() const { return comm_id_ == MPI_COMM_NULL; }

  explicit operator bool() const { return !empty(); }

  /// Communicators' underlying id
  MPI_Comm const& operator()() const { return comm_id_; }

  /// Size of communicator c
  auto size() const -> rank_t {
    int size_;
    HM3_MPI_CALL(MPI_Comm_size(comm_id_, &size_));
    return rank_t{size_};
  }

  /// Clones a communicator (calls MPI_Comm_dup)
  comm clone() const {
    MPI_Comm c;
    HM3_MPI_CALL(MPI_Comm_dup(comm_id_, &c));
    return comm{c};
  }

  explicit operator MPI_Comm() const { return comm_id_; }
  static comm world() noexcept {
    if (!mpi::initialized()) {
      int thread_support;
      HM3_MPI_CALL(
       MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &thread_support));

      if (thread_support != MPI_THREAD_MULTIPLE) {
        fmt::print(
         "[WARNING] MPI implementation does not support multi-thread\n");
      }
    }
    return comm{MPI_COMM_WORLD};
  }
  static comm invalid() noexcept { return comm{}; }
};

/// Current process rank within communicator \p c
inline auto rank(comm const& c = comm{}) {
  int rank_;
  HM3_MPI_CALL(MPI_Comm_rank(c(), &rank_));
  return rank_t{rank_};
}

/// Is the current process the root of the communicator?
inline bool is_root(const mpi::comm& comm) { return rank(comm) == root; }

namespace detail {

template <typename F, typename... Args>
constexpr void root_do_(std::true_type, const comm& comm, F&& f,
                        Args&&... args) noexcept {
  if (is_root(comm)) { f(std::forward<Args>(args)...); }
  return;
}

template <typename F, typename... Args>
constexpr auto root_do_(std::false_type, const comm& comm, F&& f,
                        Args&&... args) noexcept
 -> decltype(f(std::forward<Args>(args)...)) {
  decltype(f(std::forward<Args>(args)...)) result{};
  if (is_root(comm)) { result = f(std::forward<Args>(args)...); }
  HM3_MPI_CALL(MPI_Bcast(&result, 1, data_type(result), *root, comm()));
  return result;
}

}  // namespace detail

/// \brief Performs a computation _only_ at the root of the communicator
///
/// If the computation has a return type, this type is broadcasted to all
/// MPI Processes.
template <typename F, typename... Args>
constexpr auto root_do(const comm& comm, F&& f, Args&&... args)
 -> decltype(f(std::forward<Args>(args)...)) {
  using is_void = Same<decltype(f(std::forward<Args>(args)...)), void>;
  return detail::root_do_(is_void{}, comm, std::forward<F>(f),
                          std::forward<Args>(args)...);
}

}  // namespace mpi
}  // namespace hm3
