// #pragma once
// /// \file
// ///
// /// Vector type
// #include <hm3/geometry/dimension/dimensional.hpp>
// #include <hm3/types.hpp>
// #include <hm3/utility/math.hpp>
// #include <hm3/utility/matrix.hpp>

// namespace hm3 {
// namespace geometry {

// template <uint_t Nd>
// using vector_base_t = dense::vector<num_t, static_cast<int_t>(Nd), suint_t>;

// template <uint_t Nd>  //
// struct vector : vector_base_t<Nd>, dimensional<Nd> {
//   using self = vector_base_t<Nd>;
//   using self::self;
//   using self::operator=;
// };

// template <int_t Nd> bool operator==(vector<Nd> const& l, vector<Nd> const& r) {
//   for (sint_t d = 0; d != Nd; ++d) {
//     if (!math::approx(l(d), r(d))) { return false; }
//   }
//   return true;
// }

// template <int_t Nd> bool operator!=(vector<Nd> const& l, vector<Nd> const& r) {
//   return !(l == r);
// }

// template <typename OStream, uint_t Nd>
// OStream& operator<<(OStream& o, vector<Nd> const& p) {
//   o << "(";
//   for (suint_t d = 0; d != Nd; ++d) {
//     o << p(d);
//     if (d != Nd - 1) { o << ", "; }
//   }
//   o << ")";
//   return o;
// }

// /// Vector length
// template <int_t Nd> constexpr num_t length(vector<Nd> const& p) {
//   return p().norm();
// }


// }  // namespace geometry
// }  // namespace hm3
