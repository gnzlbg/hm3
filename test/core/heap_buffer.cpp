////////////////////////////////////////////////////////////////////////////////
/// \file Tests the Arithmetic class
#include <hom3/core/heap_buffer.hpp>
#include <hom3/core/test.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

namespace hom3 {
namespace memory {
namespace buffer {
template struct view<int>;
}  // namespace buffer
}  // namespace memory
}  // namespace hom3

/// \test Buffer usage example
void test_example() {
  // Create a memory buffer
  memory::buffer::buffer buffer;
  // To use the buffer, create a buffer::Accesor with the required size:
  auto b = memory::buffer::view<double>{buffer, 4};
  // Now you have a buffer \p that can hold 1000 doubles.
  // To access an element of the buffer you can use the () operator
  b(3) = 5.0;  // The () operator also performs out of bounds checking!
  // Buffer satisfy the container requirements, so you can iterate
  // over them using begin/end:
  for (auto&& i : b) { i *= 2.0; }
  // get a pointer to the raw buffer data:
  b.data();
  // as well as resizing the buffer, and asking the buffer size/capacity
  b.resize(10);
  CHECK(b.size() == 10);
  CHECK(b.capacity() >= b.size());
}

template <class T>
void is_filled_with(memory::buffer::view<T> const& b, const T value) {
  Ind size = 0;
  for (auto&& i : b) {
    CHECK(value == i);
    ++size;
  }
  CHECK(b.size() == size);
}

void is_filled_with(memory::buffer::view<Num> const& b, const Num value) {
  Ind size = 0;
  for (auto&& i : b) {
    CHECK(value == i);
    ++size;
  }
  CHECK(b.size() == size);
}

template <class T> void basic_buffer_test(memory::buffer::buffer& buf) {
  /// Create a buffer view of 10 Ts
  {
    memory::buffer::view<T> b{buf, 10};
    CHECK(b.capacity() == Ind{10});
    CHECK(b.size() == Ind{10});
    b.fill();
    is_filled_with(b, T{});
    b.fill(T{3});
    is_filled_with(b, T{3});
  }

  /// The bufer is cleared up using RAII
  CHECK((buf.capacity() / sizeof(T)) == std::size_t{10});
  CHECK(buf.size() == std::size_t{0});

  /// A new view causes no new allocations
  {
    memory::buffer::view<T> b{buf, 5};
    CHECK(b.capacity() == Ind{10});  // capacity should still be 10 !
    CHECK(b.size() == Ind{5});
  }
}

template <class T> void basic_buffer_test() {
  memory::buffer::buffer b;
  basic_buffer_test<T>(b);
}

void basic_tests() {
  basic_buffer_test<int64_t>();
  basic_buffer_test<uint32_t>();
  basic_buffer_test<double>();
}

void buffer_reuse() {
  memory::buffer::buffer buf;
  basic_buffer_test<int64_t>(buf);
  basic_buffer_test<double>(buf);
}

int main() {
  test_example();
  basic_tests();
  buffer_reuse();
  return test::result();
}
