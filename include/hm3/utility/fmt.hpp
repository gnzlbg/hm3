#pragma once
/// \file
///
/// Formatting utilties
#include <hm3/ext/fmt.hpp>
#include <hm3/ext/string_view.hpp>
#include <hm3/utility/ref.hpp>

/// Formatting of types on assertions.
///
/// The assert macros use the print function in this namespace to format the
/// output. This in turns dispatches to dispatch(OStream, T, tag) functions.
///
/// The one defined here is the worst possible candidate (see the trait
/// hierarchy).
///
/// If you define one with a better "rank", it will be picked instead.
///
/// For example:
///
/// - <hm3/geometry/io/ascii.hpp> defines one with a better rank that
///   knows how to serialize geometry objects to ascii.
/// - <hm3/io/ascii.hpp> defines one with the best possible rank that hopefully
///   knows how to serialize all objects to ascii (including geometry objects).
///
/// The hierarchy can be hijacked here at will without breaking anything by
/// inserting new tags wherever you see fit.
///
/// For using those tags to print stuff, a dispatch overload must exist in the
/// hm3::ascii_fmt namespace. You can add overloads to this namespace wherever
/// you want.
///
namespace hm3::ascii_fmt {

/// \name Trait hierarchy to resolve ambiguities
///@{

struct non_streamable {};
struct ostream_op : non_streamable {};
struct range_like : ostream_op {};

// Note, the order doesn't really matter, important is only that there is a
// strict order:
struct utility : range_like {};
struct geometry : utility {};
struct tree : geometry {};
struct grid : tree {};
struct solver : grid {};

struct io : solver {};

///@} // Trait hierarchy

template <typename OStream, typename T>
OStream& to_ascii(OStream& o, T const&, non_streamable const&) {
  int status;
  const char* tyname = typeid(T).name();
  char* name         = abi::__cxa_demangle(tyname, nullptr, nullptr, &status);
  o << "<type \"";
  if (name != nullptr) {
    o << name;
  } else {
    o << tyname;
  }

  o << "\" not serializable to ascii>";
  return o;
};

template <typename OStream, typename T>
auto to_ascii(OStream& o, T const& t, ostream_op const&)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(o << t);

template <typename OStream, typename T>
auto to_ascii(OStream& o, T const& t)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(to_ascii(o, t, io{}));

template <typename T>
struct wrap_t {
  ref_t<T> value;

  template <typename OStream>
  friend OStream& operator<<(OStream& o, wrap_t<T> const& t) noexcept {
    to_ascii(o, t.value.get());
    return o;
  }
};

template <typename T, typename U = uncvref_t<T>>
static constexpr bool should_wrap
 = not std::is_integral<U>{}  // and not std::is_pointer<U>{}
   and not std::is_floating_point<U>{}
   and not std::is_same<std::decay_t<T>, const char*>{}
   and not std::is_same<U, std::string>{} and not std::is_same<U, string_view>{}
   and not std::is_same<uncvref_t<std::remove_pointer_t<U>>, void>{};

template <typename T, CONCEPT_REQUIRES_(should_wrap<T>)>
wrap_t<T> wrap(T&& t) {
  return wrap_t<T>{ref_t<T>{std::forward<T>(t)}};
}

template <typename T, CONCEPT_REQUIRES_(not should_wrap<T>)>
decltype(auto) wrap(T&& t) {
  return std::forward<T>(t);
}

namespace detail {

using ::fmt::format;
using ::fmt::MemoryWriter;

template <typename Output, typename FmtString, typename... Args>
void print(Output&& o, FmtString&& s, Args&&... args) {
  ::fmt::print(
   std::forward<Output>(o),
   std::forward<  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    FmtString>(s),
   wrap(std::forward<Args>(args))...);
}

}  // namespace detail

template <typename FmtString, typename... Args>
void out(FmtString&& s, Args&&... args) {
  detail::print(stdout, std::forward<FmtString>(s),
                std::forward<Args>(args)...);
}

template <typename FmtString, typename... Args>
void err(FmtString&& s, Args&&... args) {
  detail::print(stderr, std::forward<FmtString>(s),
                std::forward<Args>(args)...);
}

}  // namespace hm3::ascii_fmt
