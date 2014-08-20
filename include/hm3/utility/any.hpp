#pragma once
/// \file
///
/// Any (with overaligned storage for Eigen3 types)
///
/// (C) Copyright Christopher Diggins 2005-2011
/// (C) Copyright Pablo Aguilar 2005
/// (C) Copyright Kevlin Henney 2001
///
/// Distributed under the Boost Software License, Version 1.0. (See
/// accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt

// Basically this Any (and Boost.Any) use new/delete for memory allocation
// instead of allcoators, this does not work well with types that need specific
// alignment like SSE intrinsics or Eigen types. If you try to access a sse type
// with the wrong alignment you get UB and a not-descriptive-at-all
// EXC_BAD_ACCESS (code=EXC_I386_GPFLT) segfaul.
//
// Define this macro to disable over-alignment of all any types and see for
// yourself:
// #define HM3_UNALIGNED_ANY_TYPE

#include <stdexcept>

namespace hm3 {
namespace any_detail {

inline void* over_aligned_new(std::size_t size) {
  constexpr std::size_t alignment_in_bytes = 64;
  void* result;
  if (posix_memalign(&result, alignment_in_bytes, size)) { result = nullptr; }

  return result;
}

inline void over_aligned_delete(void* ptr) noexcept { std::free(ptr); }

struct bad_any_cast {};

struct empty_any {};

struct base_any_policy {
  virtual void static_delete(void** x) = 0;
  virtual void copy_from_value(void const* src, void** dest) = 0;
  virtual void clone(void* const* src, void** dest) = 0;
  virtual void move(void* const* src, void** dest) = 0;
  virtual void* get_value(void** src) = 0;
  virtual size_t get_size() = 0;
};

template <typename T> struct typed_base_any_policy : base_any_policy {
  virtual size_t get_size() override { return sizeof(T); }
};

template <typename T> struct small_any_policy : typed_base_any_policy<T> {
  virtual void static_delete(void**) override final {}
  virtual void copy_from_value(void const* src, void** dest) override final {
    new (dest) T(*reinterpret_cast<T const*>(src));
  }
  virtual void clone(void* const* src, void** dest) override final {
    *dest = *src;
  }
  virtual void move(void* const* src, void** dest) override final {
    *dest = *src;
  }
  virtual void* get_value(void** src) override final {
    return reinterpret_cast<void*>(src);
  }
};

template <typename T> struct big_any_policy : typed_base_any_policy<T> {
  virtual void static_delete(void** x) override final {
    if (*x) {
#ifdef HM3_UNALIGNED_ANY_TYPE
      delete (*reinterpret_cast<T**>(x));
#else
      std::free(*reinterpret_cast<T**>(x));
#endif
    }
    *x = nullptr;
  }
  virtual void copy_from_value(void const* src, void** dest) override final {
#ifdef HM3_UNALIGNED_ANY_TYPE
    *dest = new T(*reinterpret_cast<T const*>(src));
#else
    *dest = over_aligned_new(sizeof(*reinterpret_cast<T const*>(src)));
    new (*dest) T(*reinterpret_cast<T const*>(src));
#endif
  }
  virtual void clone(void* const* src, void** dest) override final {
#ifdef HM3_UNALIGNED_ANY_TYPE
    *dest = new T(**reinterpret_cast<T* const*>(src));
#else
    *dest = over_aligned_new(sizeof(**reinterpret_cast<T* const*>(src)));
    new (*dest) T(**reinterpret_cast<T* const*>(src));
#endif
  }
  virtual void move(void* const* src, void** dest) override final {
    (*reinterpret_cast<T**>(dest))->~T();
    **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
  }
  virtual void* get_value(void** src) override final { return *src; }
};

template <typename T> struct choose_policy { typedef big_any_policy<T> type; };

template <typename T> struct choose_policy<T*> {
  typedef small_any_policy<T*> type;
};

struct any;

/// Choosing the policy for an any type is illegal, but should never happen.
/// This is designed to throw a compiler error.
template <> struct choose_policy<any> { typedef void type; };

/// Specializations for small types.
#define SMALL_POLICY(TYPE)                 \
  template <> struct choose_policy<TYPE> { \
    typedef small_any_policy<TYPE> type;   \
  };

SMALL_POLICY(signed char);
SMALL_POLICY(unsigned char);
SMALL_POLICY(signed short);
SMALL_POLICY(unsigned short);
SMALL_POLICY(signed int);
SMALL_POLICY(unsigned int);
SMALL_POLICY(signed long);
SMALL_POLICY(unsigned long);
SMALL_POLICY(float);
SMALL_POLICY(bool);

#undef SMALL_POLICY

/// This function will return a different policy for each type.
template <typename T> base_any_policy* get_policy() {
  static typename choose_policy<T>::type policy;
  return &policy;
};
}  // namespace any_detail

struct any {
 private:
  // fields
  any_detail::base_any_policy* policy;
  void* object;

 public:
  /// Initializing constructor.
  template <typename T>
  any(T const& x)
   : policy(any_detail::get_policy<any_detail::empty_any>()), object(nullptr) {
    assign(x);
  }

  /// Empty constructor.
  any() noexcept : policy(any_detail::get_policy<any_detail::empty_any>()),
                   object(nullptr) {}

  /// Special initializing constructor for string literals.
  any(const char* x)
   : policy(any_detail::get_policy<any_detail::empty_any>()), object(nullptr) {
    assign(x);
  }

  /// Copy constructor.
  any(const any& x)
   : policy(any_detail::get_policy<any_detail::empty_any>()), object(nullptr) {
    assign(x);
  }

  /// Destructor.
  ~any() noexcept { policy->static_delete(&object); }

  /// Assignment function from another any.
  any& assign(const any& x) {
    reset();
    policy = x.policy;
    policy->clone(&x.object, &object);
    return *this;
  }

  /// Assignment function.
  template <typename T> any& assign(T const& x) {
    reset();
    policy = any_detail::get_policy<T>();
    policy->copy_from_value(&x, &object);
    return *this;
  }

  /// Assignment operator.
  template <typename T> any& operator=(const T& x) { return assign(x); }

  /// Assignment operator, specialed for literal strings.
  /// They have types like const char [6] which don't work as expected.
  any& operator=(const char* x) { return assign(x); }

  /// Utility functions
  any& swap(any& x) {
    using std::swap;
    swap(policy, x.policy);
    swap(object, x.object);
    return *this;
  }

  /// Cast operator. You can only cast to the original type.
  template <typename T> T& cast() {
    if (policy != any_detail::get_policy<T>()) throw any_detail::bad_any_cast();
    T* r = reinterpret_cast<T*>(policy->get_value(&object));
    return *r;
  }

  /// Returns true if the any contains no value.
  bool empty() const {
    return policy == any_detail::get_policy<any_detail::empty_any>();
  }

  /// Frees any allocated memory, and sets the value to nullptr.
  void reset() {
    policy->static_delete(&object);
    policy = any_detail::get_policy<any_detail::empty_any>();
  }

  /// Returns true if the two types are the same.
  bool compatible(const any& x) const { return policy == x.policy; }
};

template <typename T> T& any_cast(any& a) {
  using U = std::remove_reference_t<T>;
  return a.template cast<U>();
}

template <typename T> inline T const& any_cast(const any& a) {
  using U = std::remove_cv_t<std::remove_reference_t<T>>;
  return const_cast<any&>(a).template cast<U>();
}

template <typename T> inline T any_cast(any&& a) {
  static_assert(std::is_rvalue_reference<T&&>{}
                 || std::is_const<std::remove_reference_t<T>>{},
                "any_cast shall not be used for getting nonconst references to "
                "temporary objects");
  return any_cast<T>(a);
}

}  // namespace hm3
