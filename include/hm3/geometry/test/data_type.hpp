#pragma once
/// \file
///
/// Data type mainly useful for testing algorithms on primitives storing data.
#include <cassert>

namespace hm3::geometry {

class data_type {
  int value = 42;

 public:
  friend bool operator==(data_type const& a, data_type const& b) {
    return a.value == b.value;
  }
  friend bool operator!=(data_type const& a, data_type const& b) {
    return !(a == b);
  }
  template <typename OS>
  friend OS& operator<<(OS& os, data_type const& v) {
    if (v.value == 42) {
      os << "default-constructed";
    } else {
      os << v.value;
    }
    return os;
  }
  data_type() = default;
  data_type(int i) : value(i) {
    assert(i != 42 && "error: 42 is used as uninitialized value");
  }
  data_type(data_type const&) = default;
  data_type(data_type&&)      = default;
  data_type& operator=(data_type const&) = default;
  data_type& operator=(data_type&&) = default;
};

}  // namespace hm3::geometry
