#pragma once
#include <iostream>
#include <hm3/types.hpp>
#include <hm3/utility/fmt.hpp>

namespace hm3 {
namespace debug_detail {

inline auto to_string(string&& s) -> string { return s; }
inline auto to_string(string const& s) -> string { return s; }
inline auto to_string(string& s) -> string& { return s; }

template <class T> decltype(auto) to_string(T&& t) {
  using std::to_string;
  return to_string(std::forward<T>(t));
}

}  // namespace debug_detail
}  // namespace hm3

#define _OUTPUT_CNT(...)                                                      \
  _OUTPUT_CNT2(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, \
               7, 6, 5, 4, 3, 2, 1, 0)
#define _OUTPUT_CNT2(a20, a19, a18, a17, a16, a15, a14, a13, a12, a11, a10, \
                     a9, a8, a7, a6, a5, a4, a3, a2, a1, a0, ...)           \
  a0
#define _OUTPUT_RUN(func, ...) _OUTPUT_RUN2(func, _OUTPUT_CNT(__VA_ARGS__))
#define _OUTPUT_RUN2(func, n) _OUTPUT_RUN3(func, n)
#define _OUTPUT_RUN3(func, n) func##n

#define OUTPUT_M(x) #x << "= " << ::hm3::debug_detail::to_string(x) << "; "
#define OUTPUT_1(x) #x << "= " << ::hm3::debug_detail::to_string(x) << ";\n"
#define OUTPUT_2(x, ...) OUTPUT_M(x) << OUTPUT_1(__VA_ARGS__)
#define OUTPUT_3(x, ...) OUTPUT_M(x) << OUTPUT_2(__VA_ARGS__)
#define OUTPUT_4(x, ...) OUTPUT_M(x) << OUTPUT_3(__VA_ARGS__)
#define OUTPUT_5(x, ...) OUTPUT_M(x) << OUTPUT_4(__VA_ARGS__)
#define OUTPUT_6(x, ...) OUTPUT_M(x) << OUTPUT_5(__VA_ARGS__)
#define OUTPUT_7(x, ...) OUTPUT_M(x) << OUTPUT_6(__VA_ARGS__)
#define OUTPUT_8(x, ...) OUTPUT_M(x) << OUTPUT_7(__VA_ARGS__)
#define OUTPUT_9(x, ...) OUTPUT_M(x) << OUTPUT_8(__VA_ARGS__)
#define OUTPUT_10(x, ...) OUTPUT_M(x) << OUTPUT_9(__VA_ARGS__)
#define OUTPUT_11(x, ...) OUTPUT_M(x) << OUTPUT_10(__VA_ARGS__)
#define OUTPUT_12(x, ...) OUTPUT_M(x) << OUTPUT_11(__VA_ARGS__)
#define OUTPUT_13(x, ...) OUTPUT_M(x) << OUTPUT_12(__VA_ARGS__)
#define OUTPUT_14(x, ...) OUTPUT_M(x) << OUTPUT_13(__VA_ARGS__)
#define OUTPUT_15(x, ...) OUTPUT_M(x) << OUTPUT_14(__VA_ARGS__)
#define OUTPUT_16(x, ...) OUTPUT_M(x) << OUTPUT_15(__VA_ARGS__)
#define OUTPUT_17(x, ...) OUTPUT_M(x) << OUTPUT_16(__VA_ARGS__)
#define OUTPUT_18(x, ...) OUTPUT_M(x) << OUTPUT_17(__VA_ARGS__)
#define OUTPUT_19(x, ...) OUTPUT_M(x) << OUTPUT_18(__VA_ARGS__)
/// @brief prints variable=value into a stream
///
/// @code
/// DEB // works with any ostream
///  << OUTPUT(omega,t,cos(omega*t))
///  << OUTPUT(myvec,mymap,mypair)
///  ;
/// @endcode
///
/// prints
/// @code
/// =========
/// main.cc:122: in main(int,char**):
/// omega= 0.5; t= 0.2; cos(omega*t)= 0.9950041652780258;
/// myvec= [1, 2, 3]; mymap= {'1':1, '2':2, '3':3}; mypair= (1,2);
/// @endcode
#define OUTPUT(...)                                                           \
  ::hm3::fmt::print("{}", (::hm3::fmt::MemoryWriter{}                         \
                           << _OUTPUT_RUN(OUTPUT_, __VA_ARGS__)(__VA_ARGS__)) \
                           .str())
