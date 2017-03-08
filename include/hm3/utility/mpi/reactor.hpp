#pragma once
/// \file
///
/// Non-blocking reactor.
#include <hm3/utility/flat_set.hpp>
#include <hm3/utility/mpi/comm.hpp>
#include <hm3/utility/mpi/data_type.hpp>
#include <hm3/utility/mpi/request.hpp>

namespace hm3::mpi {

/// MPI reactor.
struct reactor {
  static constexpr std::size_t expected_no_requests = 100;

  reactor(comm c) : comm_(c) {}

  comm comm_;
  small_flat_set<request, expected_no_requests> requests_;

  template <typename T>
  void send(T const* const buf, uint_t count, rank_t destination, tag t) {
    HM3_ASSERT(destination, "invalid rank");
    HM3_ASSERT(buf != nullptr, "invalid buffer");
    HM3_ASSERT(destination != rank(comm_),
               "trying to send something to yourself");
    auto dt         = data_type(*buf);
    auto p          = requests_.insert(request(t));
    auto request_it = p.first;
    auto inserted   = p.second;
    HM3_ASSERT(inserted, "unfinished request \"{}\" already exists", t.name());

    HM3_MPI_CALL(MPI_Issend(static_cast<void*>(const_cast<T*>(buf)),
                            static_cast<int>(count), dt, *destination,
                            t.value(), comm_(),
                            const_cast<MPI_Request*>((&request_it->request_))));
  }

  template <typename C,
            CONCEPT_REQUIRES_(ranges::LvalueContainerLike<C const&>{}
                              and RandomAccessRange<C>{})>
  void send(C const& rng, rank_t destination, tag t) {
    send(rng.data(), ranges::size(rng), destination, t);
  }

  template <typename V, CONCEPT_REQUIRES_(not RandomAccessRange<V>{}
                                          and not std::is_pointer<V>{})>
  void send(V const& value, rank_t destination, tag t) {
    send(&value, 1, destination, t);
  }

  bool test(tag t) noexcept {
    auto it = requests_.find(t);
    HM3_ASSERT(it != ranges::end(requests_), "request {} not found", t.name());

    int completed;
    call_s(HM3_AT_, MPI_Test, const_cast<int*>(&(it->request_)), &completed);
    return completed != 0;
  }

  void wait(tag t) noexcept {
    auto it = requests_.find(t);
    HM3_ASSERT(it != ranges::end(requests_), "request {} not found", t.name());
    if (it->request_ != MPI_REQUEST_NULL) {
      call_s(HM3_AT_, MPI_Wait, const_cast<int*>(&(it->request_)));
    }
    requests_.erase(it);
  }

  void wait_all() noexcept {
    while (not requests_.empty()) { wait(requests_.back()); }
  }

  template <typename T>
  void receive(T* buf, uint_t count, rank_t source, tag t) {
    HM3_ASSERT(source, "invalid rankk");
    HM3_ASSERT(buf != nullptr, "invalid buffer");
    HM3_ASSERT(source != rank(comm_),
               "trying to receive something from yourself");
    auto dt         = data_type(*buf);
    auto p          = requests_.insert(request(t));
    auto request_it = p.first;
    auto inserted   = p.second;
    HM3_ASSERT(inserted, "unfinished request \"{}\" already exists", t.name());

    HM3_MPI_CALL(MPI_Irecv(static_cast<void*>(buf), static_cast<int>(count), dt,
                           *source, t.value(), comm_(),
                           const_cast<MPI_Request*>(&(request_it->request_))));
  }

  template <typename C, CONCEPT_REQUIRES_(ranges::LvalueContainerLike<C&>{}
                                          and RandomAccessRange<C>{})>
  void receive(C& rng, rank_t source, tag t) {
    receive(rng.data(), ranges::size(rng), source, t);
  }

  template <typename V, CONCEPT_REQUIRES_(not RandomAccessRange<V>{}
                                          and not std::is_pointer<V>{})>
  void receive(V& value, rank_t source, tag t) {
    receive(&value, 1, source, t);
  }
};

}  // namespace hm3::mpi
