#pragma once
/// \file allocator.hpp Stack allocator
///
/// Heavily inspired on code by Howard Hinnant:
/// - http://home.roadrunner.com/~hinnant/short_alloc.h
/// - http://home.roadrunner.com/~hinnant/stack_alloc.html
#include <hm3/utility/stack/arena.hpp>

namespace hm3 {

/// Stack memory allocation utilities
namespace stack {

/// Stack allocator: obtains memory from a stack arena
///
/// \tparam T is the type of the elements to be allocated
/// \tparam N is the max. number of elements that can be _allocated
template <class T, std::size_t N> struct allocator {
  using value_type = T;

  template <class _Up> struct rebind { using other = allocator<_Up, N>; };

  allocator(arena<T, N>& a) noexcept : a_(a) {}
  template <class U> allocator(const allocator<U, N>& a) noexcept : a_(a.a_) {}
  allocator(const allocator&) = default;
  allocator& operator=(const allocator&) = delete;

  T* allocate(std::size_t n) noexcept { return a_.allocate(n); }
  void deallocate(T* p, std::size_t n) noexcept { a_.deallocate(p, n); }

  template <class T1, std::size_t N1, class U, std::size_t M>
  friend bool operator==(const allocator<T1, N1>& x,
                         const allocator<U, M>& y) noexcept;

  template <class U, std::size_t M> friend struct allocator;

 private:
  arena<T, N>& a_;
};

template <class T, std::size_t N, class U, std::size_t M>
inline bool operator==(const allocator<T, N>& x,
                       const allocator<U, M>& y) noexcept {
  return N == M && &x.a_ == &y.a_;
}

template <class T, std::size_t N, class U, std::size_t M>
inline bool operator!=(const allocator<T, N>& x,
                       const allocator<U, M>& y) noexcept {
  return !(x == y);
}

}  // namespace stack
}  // namespace hm3
