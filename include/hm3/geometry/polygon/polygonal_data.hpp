// #pragma once
// /// \file
// ///
// /// Polygonal data
// namespace hm3 {
// namespace geometry {

// template <uint_t Nd, uint_t MaxNp, typename PointData>
// struct polygonal_data : polygon<Nd, MaxNp> {
//   using self = polygon<Nd, MaxNp>;
//   using self::self;
//   using self::operator=;
//   stack::vector<PointData, MaxNp> data;
// };

// template <uint_t Nd, uint_t MaxNp, typename Pointdata>
// bool operator==(polygonal_data<Nd, MaxNp, PointData> const& a,
//                 polygonal_data<Nd, MaxNp, PointData> const& b) noexcept {
//   using base = polygon<Nd, MaxNp>;
//   return equal(static_cast<base const&>(a), static_cast<base const&>(b))
//          and equal(a.data, b.data);
// }

// template <uint_t Nd, uint_t MaxNp, typename PointData>
// bool operator!=(polygonal_data<Nd, MaxNp, PointData> const& a,
//                 polygonal_data<Nd, MaxNp, PointData> const& b) noexcept {
//   return !(a == b);
// }

// /// Corner points
// template <uint_t Nd, uint_t MaxNp, typename PointData>
// constexpr auto corners(polygonal_data<Nd, MaxNp, PointData> p) noexcept {
//   return p;
// }

// /// Corner positions
// template <uint_t Nd, uint_t MaxNp, typename PointData>
// auto corner_positions(polygonal_data<Nd, MaxNp, PointData> const& p) noexcept
// {
//   return view::iota(std::size_t{0}, p.size());
// }

// }  // namespace geometry
// }  // namespace hm3
