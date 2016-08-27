#pragma once
/// \file
///
/// Any geometric primitive
#include <hm3/geometry/primitive/any/types.hpp>
#include <hm3/geometry/primitive/any/vertices.hpp>

namespace hm3::geometry::any_primitive {

template <dim_t Nd>
struct any : any_variant_t<Nd>, dimensional<Nd> {
  using base = any_variant_t<Nd>;
  using base::base;
  using base::operator=;

  any()           = default;
  any(any const&) = default;
  any(any&&)      = default;
  any& operator=(any const&) = default;
  any& operator=(any&&) = default;

  template <dim_t N = Nd, CONCEPT_REQUIRES_(Nd > 1 and N == Nd)>
  any(polygon_t<N>&& p) : base(std::move(p)) {}
  template <dim_t N = Nd, CONCEPT_REQUIRES_(Nd > 1 and N == Nd)>
  any(polygon_t<N> const& p) : base(p) {}
  template <dim_t N = Nd, CONCEPT_REQUIRES_(Nd > 1 and N == Nd)>
  any(polygon_t<N>& p) : base(p) {}

  any(ray<Nd> const& r) : any(segment<Nd>(r.as_segment())) {}
  any& operator=(ray<Nd> const& r) {
    (*this) = any(r);
    return (*this);
  }
  any(line<Nd> const& l) : any(segment<Nd>(l.as_segment())) {}
  any& operator=(line<Nd> const& l) {
    (*this) = any(l);
    return (*this);
  }

  dim_t rank() const noexcept {
    return visit([](auto&& i) { return dim_t{i.rank()}; }, *this);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Nd>
bool operator==(any<Nd> const& a, any<Nd> const& b) noexcept {
  return a == b;
}

template <dim_t Nd>
bool operator!=(any<Nd> const& a, any<Nd> const& b) noexcept {
  return a != b;
}

}  // namespace hm3::geometry::any_primitive
