#pragma once
/// \file
///
///
#include <hm3/utility/mpi/call.hpp>
#include <hm3/utility/mpi/config.hpp>
#include <hm3/utility/mpi/tag.hpp>

namespace hm3::mpi {

struct request : tag {
  MPI_Request request_ = MPI_REQUEST_NULL;
  request() : tag(0) {}
  request(tag t) : tag(t) {}
  request(request const&) = delete;
  request& operator=(request const&) = delete;
  request(request&& other)
   : tag(static_cast<tag&&>(other)), request_(other.request_) {
    other.request_ = MPI_REQUEST_NULL;
  }
  request& operator=(request&& other) {
    if (request_ != MPI_REQUEST_NULL) {
      HM3_MPI_CALL(MPI_Request_free(&request_));
    }
    static_cast<tag&>(*this) = static_cast<tag&&>(other);
    request_                 = other.request_;
    other.request_           = MPI_REQUEST_NULL;
    return *this;
  }

  ~request() {
    if (request_ == MPI_REQUEST_NULL) { return; }
    HM3_MPI_CALL(MPI_Request_free(&request_));
  }

  using tag::operator==;
  using tag::operator!=;
  using tag::operator<;
  using tag::operator<=;
  using tag::operator>=;
  using tag::operator>;
};

}  // namespace hm3::mpiu
