#pragma once
/// \file
///
/// Stores data in a primitive using the empty-base optimization.
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

template <typename T = void>
struct primitive_data : T {
  T& get_primitive_data() & { return static_cast<T&>(*this); }
  T const& get_primitive_data() const & { return static_cast<T const&>(*this); }
  T get_primitive_data() && { return static_cast<T&&>(*this); }
  static_assert(SemiRegular<T>{},
                "the data stored in a primitive must model SemiRegular");
  static_assert(EqualityComparable<T>{},
                "the data stored in a primitive must model EqualityComparable");
};

template <>
struct primitive_data<void> {
  void get_primitive_data() const;
};

namespace concept {

struct HasData {
  template <typename T>
  static auto requires_(T&& t)
   -> decltype(rc::valid_expr(t.get_primitive_data()));
};

}  // namespace concept

template <typename T>
using HasData = rc::models<concept::HasData, T>;

template <typename P, CONCEPT_REQUIRES_(HasData<P&&>{})>
auto data(P&& p) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(p.get_primitive_data());

}  // namespace hm3::geometry
