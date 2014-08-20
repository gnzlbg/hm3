/// \file allocator.cpp Test stack allocator
#include <hm3/utility/test.hpp>
#include <hm3/utility/stack/allocator.hpp>

using namespace hm3;

template struct stack::allocator<int, 20>;

int main() {
  stack::arena<int, 5> arena;
  stack::allocator<int, 5> allocator(arena);
  return test::result();
}
