/// \file
///
/// Tests the heap_buffer class
#include <hm3/utility/heap_buffer.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

// Explicit instantiation:
namespace hm3 {
namespace memory {
namespace buffer {
template struct view<int>;
}  // namespace buffer
}  // namespace memory
}  // namespace hm3

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
  CHECK(b.size() == 10_z);
  CHECK(b.capacity() >= b.size());
}

template <class T>
void is_filled_with(memory::buffer::view<T> const& b, const T value) {
  std::size_t c = 0;
  for (auto&& i : b) {
    CHECK(value == i);
    ++c;
  }
  CHECK(b.size() == c);
}

void is_filled_with(memory::buffer::view<num_t> const& b, const num_t value) {
  std::size_t c = 0;
  for (auto&& i : b) {
    CHECK(value == i);
    ++c;
  }
  CHECK(b.size() == c);
}

template <class T>
void basic_buffer_test(memory::buffer::buffer& buf) {
  /// Create a buffer view of 10 Ts
  {
    memory::buffer::view<T> b{buf, 10};
    CHECK(b.capacity() == 10_z);
    CHECK(b.size() == 10_z);
    b.fill();
    is_filled_with(b, T{});
    b.fill(T{3});
    is_filled_with(b, T{3});
  }

  /// The bufer is cleared up using RAII
  CHECK((buf.capacity() / sizeof(T)) == 10_z);
  CHECK(buf.size() == 0_z);

  /// A new view causes no new allocations
  {
    memory::buffer::view<T> b{buf, 5};
    CHECK(b.capacity() == 10_z);  // capacity should still be 10 !
    CHECK(b.size() == 5_z);
  }
}

template <class T>
void basic_buffer_test() {
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
