#pragma once
/// \file
///
/// Heap allocated buffer
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>
#include <vector>

namespace hm3 {
namespace memory {
namespace buffer {

using buffer = std::vector<char>;

/// Adapt's a memory buffer to the type T
template <typename T, typename Buffer = buffer> struct view {
  using value_type      = T;
  using iterator        = T*;
  using const_iterator  = T const*;
  using reference       = T&;
  using const_reference = T const&;

  view(Buffer& buffer, const uint_t noTs = 0) : buf_(buffer) { resize(noTs); }
  ~view() { buf_.clear(); }
  view()            = delete;
  view(view const&) = delete;
  view& operator=(view const&) = delete;
  view(view&&)                 = default;
  view& operator=(view&&) = default;

  /// buffer capacity to hold T's
  inline uint_t capacity() const noexcept {
    return buf_.capacity() / sizeof(T);
  }
  /// Makes sure buffer is able to hold \p noTs \p T 's.
  inline void resize(const uint_t noTs) { buf_.resize(noTs * sizeof(T)); }
  /// Makes sure buffer is able to hold \p noTs \p T 's.
  inline uint_t size() const noexcept { return buf_.size() / sizeof(T); }
  /// Access \p i 'th T element of the buffer
  inline reference operator()(const uint_t i) noexcept {
    HM3_ASSERT(i >= 0 && i < size(), "Element '{}' is out-of-bounds [0, {})", i,
               size());
    return reinterpret_cast<reference>(buf_[i * sizeof(T)]);
  }
  inline value_type operator()(const uint_t i) const noexcept {
    HM3_ASSERT(i >= 0 && i < size(), "Element '{}' is out-of-bounds [0, {})", i,
               size());
    return reinterpret_cast<reference>(buf_[i * sizeof(T)]);
  }

  inline reference operator[](const uint_t i) noexcept { return (*this)(i); }
  inline value_type operator[](const uint_t i) const noexcept {
    return (*this)(i);
  }

  /// Fill
  inline void fill(T value = T{}) noexcept {
    generate(*this, [v = std::move(value)]() { return v; });
  }

  /// Pointer to raw data
  inline iterator data() noexcept {
    return reinterpret_cast<iterator>(buf_.data());
  }
  inline const_iterator data() const noexcept {
    return reinterpret_cast<iterator>(buf_.data());
  }

  inline iterator begin() noexcept { return data(); }
  inline iterator end() noexcept { return data() + size(); }
  inline const_iterator begin() const noexcept { return data(); }
  inline const_iterator end() const noexcept { return data() + size(); }

 private:
  Buffer& buf_;
};

}  // namespace buffer
}  // namespace memory
}  // namespace hm3
