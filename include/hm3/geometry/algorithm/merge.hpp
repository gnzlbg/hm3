#pragma once
/// \file
///
/// Merge two primitives
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::geometry {

namespace merge_detail {

struct merge_fn {
  template <typename T, typename U>
  static constexpr auto merge_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge(std::forward<U>(u),
                                              std::forward<T>(t)));

  template <typename T, typename U>
  static constexpr auto merge_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge(std::forward<T>(t),
                                              std::forward<U>(u)));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(merge_impl(std::forward<T>(t),
                                                   std::forward<U>(u), 0));

  template <typename Rng, typename Vec>
  static auto range(Rng&& rng, Vec& v) {
    using primitive_t = uncvref_t<range_value_t<Rng>>;
    using target_t    = uncvref_t<range_value_t<Vec>>;
    // result of merging one primitive with the target type
    static_assert(
     Constructible<target_t, primitive_t>{},
     "error the target type cannot be constructed from primitive type");

    HM3_ASSERT(v.empty(), "the resulting vector of the merge must be empty");

    for (auto&& p_ : rng) {  // for each primitive

      auto p = target_t(p_);  // make it of the target type

      // for each target already in the vector (skipped if the vector is empty)
      for (auto&& t : v) {
        // try to merge the primitive with the target
        auto r = merge(t, p);
        if (!r) { continue; }  // if cannot be merged with the target, continue
        // otherwise, it can be merged with the target
        t = r.value();  // so replace the target with the result of the merge
        goto merged;    // and we jump to the merged part
      }

      // we can only land here if no merges happened (because the vector is
      // empty,
      // due to p being the first primitive, or because all merges failed).
      // Anyways, we add the primitive to the vector and go to the next
      // primitive:
      v.push_back(p);
      continue;

    merged:
      // we can only land here if the primitive was succesfully merged with a
      // target of the vector:
      //
      // this might have enabled new merges between target of the vector, that
      // might enable new merges as well.
      //
      // so we try to remerge all targets of the vector among themselves until
      // no
      // more merges happen
      //
      // TODO: this loop is very bad
      // -it should be performed after all primitives have been inserted, and
      //  it should be linear
      bool done = false;
      while (not done) {
        done = true;
        for (auto b1 = begin(v), e = end(v); b1 != e; ++b1) {
          for (auto b2 = b1 + 1; b2 != e; ++b2) {
            auto r = merge(*b1, *b2);
            if (!r) { continue; }
            (*b1) = r.value();
            v.erase(b2);
            b1   = e;
            done = false;
          }
        }
      }
    }
  }
};

}  // namespace merge_detail

namespace {
static constexpr auto const& merge
 = static_const<merge_detail::merge_fn>::value;
}  // namespace

}  // namespace hm3::geometry
