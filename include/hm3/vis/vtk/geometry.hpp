#pragma once
/// \file
///
/// Maps different cell type geometries
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

template <uint_t Nd> struct supported_geometries {
  using type = std::experimental::variant<geometry::square<Nd>>;
};

template <> struct supported_geometries<2_u> {
  using type = std::experimental::variant<geometry::square<2_u>,
                                          geometry::polygon<2_u, 5>>;
};

template <uint_t Nd> using geometries = typename supported_geometries<Nd>::type;

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
