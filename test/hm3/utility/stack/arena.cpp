/// \file arena.cpp Test stack arena
#include <hm3/utility/test.hpp>
#include <hm3/utility/stack/arena.hpp>
#include <hm3/utility/range.hpp>

using namespace hm3;

template struct stack::arena<int, 20>;

int main() {
  stack::arena<int, 5> a;
  CHECK(a.capacity() == 5u);

  int* b = begin(a);
  b[0]   = 0;
  b[1]   = 1;
  b[2]   = 2;  // this writes are safe to the stack
  b[3]   = 3;  // but are past the allocated memory
  b[4]   = 4;

  // Allocation tests:
  auto p = a.allocate(2);
  static_assert(std::is_same<decltype(p), int*>::value, "");
  CHECK(a.size() == 2u);
  CHECK(p == b);
  CHECK(p[0] == 0);
  CHECK(p[1] == 1);

  p = a.allocate(2);
  CHECK(a.size() == 4u);
  CHECK(p[0] == 2);
  CHECK(p[1] == 3);
  CHECK(p == (b + 2));

  p = a.allocate(1);
  CHECK(a.size() == 5u);
  CHECK(p[0] == 4);
  CHECK(p == (b + 4));

  p = a.allocate(0);
  CHECK(a.size() == 5u);
  CHECK(p == nullptr);

  // Dealocation tests:
  a.deallocate(end(a), 0);
  CHECK(a.size() == 5u);

  a.deallocate(begin(a), 5);
  CHECK(a.size() == 0u);
  a.allocate(5);
  CHECK(a.size() == 5u);

  a.deallocate(begin(a) + 2, 3);
  CHECK(a.size() == 2u);

  /// Constructors:
  {
    auto c = a;
    auto i = begin(c);
    CHECK(i[0] == 0);
    CHECK(i[1] == 1);
    CHECK(i[2] == 2);
    CHECK(i[3] == 3);
    CHECK(i[4] == 4);

    auto c2(a);
    i = begin(c2);
    CHECK(i[0] == 0);
    CHECK(i[1] == 1);
    CHECK(i[2] == 2);
    CHECK(i[3] == 3);
    CHECK(i[4] == 4);

    i[0] = 5;

    c = c2;

    i[0] = 0;

    i = begin(c2);
    CHECK(i[0] == 0);
    CHECK(i[1] == 1);
    CHECK(i[2] == 2);
    CHECK(i[3] == 3);
    CHECK(i[4] == 4);

    i = begin(c);
    CHECK(i[0] == 5);
    CHECK(i[1] == 1);
    CHECK(i[2] == 2);
    CHECK(i[3] == 3);
    CHECK(i[4] == 4);
  }

  return test::result();
}
