#pragma once
/// \file arena.hpp Stack arena
///
/// Inspired on code by Howard Hinnant:
/// - http://home.roadrunner.com/~hinnant/short_alloc.h
/// - http://home.roadrunner.com/~hinnant/stack_alloc.html
#include <array>
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/terminate.hpp>

namespace hm3 {

namespace stack {

/// Stack arena (block of stack memory)
///
/// \tparam T is the type of the elements allocated
/// \tparam N is the number of elements allocated
///
///  | 0 | 1 | 2 | X | X | X |  |
///    ^begin      ^last      ^end
///
template <class T, std::size_t N> struct arena {
  using value_type      = T;
  using pointer_type    = T*;
  using iterator        = T*;
  using const_iterator  = const T*;
  using reference       = T&;
  using const_reference = T const&;

  arena() noexcept : last_(std::begin(buffer_)) {}
  arena(const arena& o) noexcept : buffer_(o.buffer_),
                                   last_(std::begin(buffer_) + o.size()) {}
  arena& operator=(const arena& o) noexcept {
    buffer_ = o.buffer_;
    last_   = std::begin(buffer_) + o.size();
    return *this;
  }
  arena(arena&&) = delete;
  arena& operator=(arena&&) = delete;
  ~arena() = default;

  /// Allocates n elements
  T* allocate(std::size_t n) noexcept {
    if (n == std::size_t{0}) { return nullptr; }
    if (last_ + n <= std::end(buffer_)) {
      auto old_last = last_;
      last_ += n;
      return old_last;
    }

    HM3_TERMINATE("Arena allocator ran out of stack memory:\n  capacity: "
                  "{},\n  size: {},\n  requested: {} (exceeds capacity by {}).",
                  capacity(), size(), n, (last_ + n - end()));
  }

  /// Deallocates n elements starting at the element p
  void deallocate(T* p, std::size_t n) noexcept {
    HM3_ASSERT(p >= std::begin(buffer_) && p <= std::end(buffer_),
               "p doesn't refer to an element in the arena");
    HM3_ASSERT(
     p + n == std::end(buffer_),
     "trying to deallocate elements past or in the middle of the arena");
    last_ -= n;
  };

  /// Capacity
  static constexpr std::size_t capacity() noexcept { return N; }
  /// Size
  std::size_t size() const noexcept { return last_ - std::begin(buffer_); }
  /// First
  T* begin() { return std::begin(buffer_); }
  /// One past the end
  T* end() { return last_; }

 private:
  static constexpr std::size_t alignment = 64;
  alignas(alignment) std::array<T, N> buffer_;
  T* last_;
};

}  // namespace stack
}  // namespace hm3
