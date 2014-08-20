#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Stack allocated vector
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <vector>
#include <hom3/core/stack/allocator.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {
namespace stack {

/// Stack allocated vector
/// \tparam T is the type of the elements stored in the vector
/// \tparam N is the max. number of elements that can be _allocated_ by the
///           vector and, due to std::vector growth rules, it might need to
///           be larger than the desired max. number of elements.
template <class T, std::size_t N>
struct vector : private stack::arena<T, N>,
                public std::vector<T, stack::allocator<T, N>> {
  using allocator_t = stack::allocator<T, N>;
  using arena_t = stack::arena<T, N>;
  using base_vector = std::vector<T, allocator_t>;
  using other = std::vector<T>;

  using base_vector::operator=;  // Inherit std::vector assignment
  using base_vector::size;

  using base_vector::begin;
  using base_vector::end;
  using base_vector::capacity;

  using value_type = typename base_vector::value_type;
  using reference = typename base_vector::reference;
  using const_reference = typename base_vector::const_reference;
  using iterator = typename base_vector::iterator;
  using const_iterator = typename base_vector::const_iterator;
  using pointer = typename base_vector::pointer;
  using const_pointer = typename base_vector::const_pointer;
  using size_type = typename base_vector::size_type;
  using difference_type = typename base_vector::difference_type;
  using reverse_iterator = typename base_vector::reverse_iterator;
  using const_reverse_iterator = typename base_vector::const_reverse_iterator;
  using allocator_type = allocator_t;

  /// \name Forward std::vector constructors
  ///@{
  inline explicit vector(typename base_vector::size_type count = 0,
                         const T& value = T{})
   : base_vector(count, value, allocator()) {
    (*this).reserve(N);
  }

  inline vector(const vector& other) : arena_t(), base_vector(allocator()) {
    (*this).reserve(N);
    for (auto&& i : other) { (*this).push_back(i); }
  }
  inline vector(vector&& other) : arena_t(), base_vector(allocator()) {
    (*this).reserve(N);
    for (auto&& i : other) { (*this).emplace_back(std::move_if_noexcept(i)); }
  }
  inline vector(std::initializer_list<T> init) : base_vector(allocator()) {
    ASSERT(init.size() <= N, "initializer list exceeds allocator capacity!");
    (*this).reserve(N);  // in case init is smaller than N
    for (auto&& i : init) { (*this).emplace_back(std::move_if_noexcept(i)); }
  }
  inline vector(const other& other) : base_vector(allocator()) {
    ASSERT(other.size() <= N, "other exceeds allocator capacity!");
    (*this).reserve(N);
    for (auto&& i : other) { (*this).push_back(i); }
  }
  inline vector(other&& other) : base_vector(allocator()) {
    ASSERT(other.size() <= N, "other exceeds allocator capacity!");
    (*this).reserve(N);
    for (auto&& i : other) { (*this).emplace_back(std::move_if_noexcept(i)); }
  }
  ///@}

 private:
  inline allocator_t allocator() { return *static_cast<arena_t*>(this); }
};

}  // namespace stack
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
