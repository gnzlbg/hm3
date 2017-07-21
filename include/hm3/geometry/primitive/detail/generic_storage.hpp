#pragma once
/// \file
///
/// Generic storage: adapts fixed, small, normal vectors and std::arrays, as a
/// conditionally pushbackable, popbackable, and/or reservable container.
#include <hm3/utility/range.hpp>

namespace hm3::geometry::detail {

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
struct generic_storage : Storage {
  using element_type = ranges::value_type_t<Storage>;
  using storage_t    = Storage;

  using storage_t::storage_t;  // NOLINT(modernize-use-equals-default)
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
            CONCEPT_REQUIRES_(PushBackable<storage_t, element_type>{})>
  constexpr auto insert(Args&&... args) {
    return static_cast<storage_t*>(this)->insert(std::forward<Args>(args)...);
  }

 private:
  // Multi pass range that can be inserted into "this" but that does not equal
  // self.
  template <typename R>
  using multi_pass_insertable
   = meta::and_<Range<R>, meta::not_<ranges::SinglePass<R>>,
                meta::not_<ConvertibleTo<R, generic_storage>>,
                Constructible<element_type, ranges::range_reference_t<R>>>;

 public:
  template <
   typename Elements,
   CONCEPT_REQUIRES_(multi_pass_insertable<Elements>{}
                     and PushBackable<storage_t, range_value_t<Elements>>{})>
  constexpr generic_storage& operator=(Elements&& vs) noexcept {
    auto no_elements = ranges::distance(vs);
    HM3_ASSERT(no_elements <= max_size(),
               "number of elements {} exceeds max_size {}!\n\nelements:\n\n{}",
               no_elements, this->max_size() /*, view::all(vs) */);
    this->clear();
    reserve(no_elements);
    for (auto&& v : std::forward<Elements>(vs)) { this->push_back(v); }
    return (*this);
  }

  template <typename Elements,
            CONCEPT_REQUIRES_(
             multi_pass_insertable<Elements>{}
             and not PushBackable<storage_t, range_value_t<Elements>>{})>
  constexpr generic_storage& operator=(Elements&& vs) noexcept /*TODO*/ {
    auto no_elements = ranges::distance(vs);
    HM3_ASSERT(no_elements == size(),
               "number of elements {} != fixed-size "
               "number of elements in storage "
               "{}!\n\nelements:\n\n{}",
               no_elements, this->size() /*, view::all(vs) */);

    ranges::copy(vs, this->begin());
    return (*this);
  }

  template <typename Elements,
            CONCEPT_REQUIRES_(multi_pass_insertable<Elements>{})>
  explicit constexpr generic_storage(Elements&& vs) noexcept /*TODO*/ {
    (*this) = std::forward<Elements>(vs);
  }

  storage_t const& elements() const & {
    return static_cast<storage_t const&>(*this);
  }
  storage_t& elements() & { return static_cast<storage_t&>(*this); }
  storage_t elements() && { return static_cast<storage_t&&>(*this); }
};

}  // namespace hm3::geometry::detail
