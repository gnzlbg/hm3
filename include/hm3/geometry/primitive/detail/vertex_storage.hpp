#pragma once
/// \file
///
/// Dynamic vertex storage
#include <hm3/utility/range.hpp>
#include <iostream>

namespace hm3::geometry {

/// Implementation detail of vertices storage
namespace vertex_storage_detail {

namespace concepts {

struct pushbackable {
  template <typename T, typename U>
  static auto requires_(T&& t, U&& u) -> decltype(  //
   rc::valid_expr(((void)t.push_back(std::forward<U>(u)), 42)));
};

using PushBackable = pushbackable;

struct popbackable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(((void)t.pop_back(), 42)));
};

using PopBackable = popbackable;

struct reservable {
  template <typename T>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(((void)t.reserve(0), 42)));
};

using Reservable = reservable;

}  // namespace concepts

template <typename T, typename U>
using PushBackable
 = rc::models<concepts::PushBackable, uncvref_t<T>, uncvref_t<U>>;

template <typename T>
using PopBackable = rc::models<concepts::PopBackable, uncvref_t<T>>;

template <typename T>
using Reservable = rc::models<concepts::Reservable, uncvref_t<T>>;

template <typename Storage>
struct vertex_storage : Storage {
  using vertex_type = typename Storage::value_type;
  using storage_t   = Storage;

  using storage_t::storage_t;
  using storage_t::operator=;
  using storage_t::size;
  using storage_t::empty;
  using storage_t::max_size;
  using storage_t::operator[];
  using storage_t::begin;
  using storage_t::end;

 public:
  CONCEPT_REQUIRES(Reservable<storage_t>{})
  constexpr auto reserve(suint_t n) noexcept {
    return static_cast<storage_t*>(this)->reserve(n);
  }

  template <typename T, CONCEPT_REQUIRES_(PushBackable<storage_t, T>{})>
  constexpr auto push_back(T&& t) {
    return static_cast<storage_t*>(this)->push_back(std::forward<T>(t));
  }

  CONCEPT_REQUIRES(PopBackable<storage_t>{})
  constexpr auto pop_back() noexcept {
    return static_cast<storage_t*>(this)->pop_back();
  }

  template <typename... Args,
            CONCEPT_REQUIRES_(PushBackable<storage_t, vertex_type>{})>
  constexpr auto insert(Args&&... args) {
    return static_cast<storage_t*>(this)->insert(std::forward<Args>(args)...);
  }

 private:
  // Multi pass range that can be inserted into "this" but that does not equal
  // self.
  template <typename R>
  using multi_pass_insertable
   = meta::and_<Range<R>, meta::not_<ranges::SinglePass<R>>,
                meta::not_<ConvertibleTo<R, vertex_storage>>,
                Constructible<vertex_type, ranges::range_reference_t<R>>>;

 public:
  template <
   typename Vertices,
   CONCEPT_REQUIRES_(multi_pass_insertable<Vertices>{}
                     and PushBackable<storage_t, range_value_t<Vertices>>{})>
  constexpr vertex_storage& operator=(Vertices&& vs) noexcept /*TODO*/ {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices <= max_size(),
               "number of vertices {} exceeds max_size {}!\n\nvertices:\n\n{}",
               no_vertices, this->max_size() /*, view::all(vs) */);
    this->clear();
    reserve(no_vertices);
    for (auto&& v : std::forward<Vertices>(vs)) { this->push_back(v); }
    return (*this);
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(
             multi_pass_insertable<Vertices>{}
             and not PushBackable<storage_t, range_value_t<Vertices>>{})>
  constexpr vertex_storage& operator=(Vertices&& vs) noexcept /*TODO*/ {
    auto no_vertices = ranges::distance(vs);
    HM3_ASSERT(no_vertices == size(),
               "number of vertices {} != fixed-size "
               "number of vertices in storage "
               "{}!\n\nvertices:\n\n{}",
               no_vertices, this->size() /*, view::all(vs) */);

    ranges::copy(vs, this->begin());
    return (*this);
  }

  template <typename Vertices,
            CONCEPT_REQUIRES_(multi_pass_insertable<Vertices>{})>
  constexpr vertex_storage(Vertices&& vs) noexcept /*TODO*/ {
    (*this) = std::forward<Vertices>(vs);
  }

  storage_t const& vertices() const & {
    return static_cast<storage_t const&>(*this);
  }
  storage_t& vertices() & { return static_cast<storage_t&>(*this); }
  storage_t vertices() && { return static_cast<storage_t&&>(*this); }
};

}  // namespace vertex_storage_detail
}  // namespace hm3::geometry
