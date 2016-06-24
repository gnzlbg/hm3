#pragma once
// \file units.hpp Unit and Quantity types
//#define HM3_USE_UNITS
#ifdef HM3_USE_UNITS
#include <hm3/types.hpp>
#include <hm3/utility/boost/units.hpp>

namespace boost {

namespace units {

/// Derived dimension for energy density: L^-1, M, T^-2
using energy_density_dimension
 = derived_dimension<length_base_dimension, -1, mass_base_dimension, 1,
                     time_base_dimension, -2>::type;

/// Derived dimension for specific energy: L^2, T^-2
using specific_energy_dimension
 = derived_dimension<length_base_dimension, 2, time_base_dimension, -2>::type;

/// Derived dimension for the dynamic viscosity: M, L^-1, T^-1
using dynamic_viscosity_dimension
 = derived_dimension<mass_base_dimension, 1, length_base_dimension, -1,
                     time_base_dimension, -1>::type;

/// Derived dimension for the thermal diffusivity: L^2, T^-1
using thermal_diffusivity_dimension
 = derived_dimension<length_base_dimension, 2, time_base_dimension, -1>::type;

/// Derived dimension for the specific gas constant: L^2, T^-2, K^-1
using specific_gas_constant_dimension
 = derived_dimension<length_base_dimension, 2, time_base_dimension, -2,
                     temperature_base_dimension, -1>::type;

namespace si {

using energy_density = unit<energy_density_dimension, si::system>;
BOOST_UNITS_STATIC_CONSTANT(joule_per_cubic_meter, energy_density);

using specific_energy = unit<specific_energy_dimension, si::system>;
BOOST_UNITS_STATIC_CONSTANT(joule_per_kilogram, specific_energy);

using dynamic_viscosity = unit<dynamic_viscosity_dimension, si::system>;
BOOST_UNITS_STATIC_CONSTANT(kg_per_meter_per_second, dynamic_viscosity);

using thermal_diffusivity = unit<thermal_diffusivity_dimension, si::system>;
BOOST_UNITS_STATIC_CONSTANT(meter_squared_per_second, thermal_diffusivity);

using specific_gas_constant = unit<specific_gas_constant_dimension, si::system>;
BOOST_UNITS_STATIC_CONSTANT(joule_per_kilogram_kelvin, specific_gas_constant);

}  // namespace si

#define DECLARE_BASE_UNIT_INFO(base_unit_, name_, symbol_) \
  template <> struct base_unit_info<base_unit_> {          \
    static const char* name() { return name_; }            \
    static const char* symbol() { return symbol_; }        \
  };

struct data_capacity_base_dimension
 : base_dimension<data_capacity_base_dimension, 73254> {};
typedef data_capacity_base_dimension::dimension_type data_capacity_dimension;

struct bit_base_unit
 : base_unit<bit_base_unit, data_capacity_dimension, 76389> {};

typedef scaled_base_unit<bit_base_unit, scale<2, static_rational<3>>>
 byte_base_unit;
typedef scaled_base_unit<byte_base_unit, scale<2, static_rational<10>>>
 kilobyte_base_unit;
typedef scaled_base_unit<byte_base_unit, scale<2, static_rational<20>>>
 megabyte_base_unit;
typedef scaled_base_unit<byte_base_unit, scale<2, static_rational<30>>>
 gigabyte_base_unit;
typedef scaled_base_unit<byte_base_unit, scale<2, static_rational<40>>>
 terabyte_base_unit;
typedef scaled_base_unit<byte_base_unit, scale<2, static_rational<50>>>
 petabyte_base_unit;

DECLARE_BASE_UNIT_INFO(bit_base_unit, "bit", "b")
DECLARE_BASE_UNIT_INFO(byte_base_unit, "byte", "B")
DECLARE_BASE_UNIT_INFO(kilobyte_base_unit, "kilobyte", "KB")
DECLARE_BASE_UNIT_INFO(megabyte_base_unit, "megabyte", "MB")
DECLARE_BASE_UNIT_INFO(gigabyte_base_unit, "gigabyte", "GB")
DECLARE_BASE_UNIT_INFO(terabyte_base_unit, "terabyte", "TB")
DECLARE_BASE_UNIT_INFO(petabyte_base_unit, "petabyte", "PB")

BOOST_UNITS_STATIC_CONSTANT(bit, bit_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(bits, bit_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(byte, byte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(bytes, byte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(kilobyte, kilobyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(kilobytes, kilobyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(megabyte, megabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(megabytes, megabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(gigabyte, gigabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(gigabytes, gigabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(terabyte, terabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(terabytes, terabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(petabyte, petabyte_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(petabytes, petabyte_base_unit::unit_type);

#undef DECLARE_BASE_UNIT_INFO

}  // namespace units
}  // namespace boost

namespace hm3 {

/// Quantity Types
namespace quantity {

namespace busi = boost::units::si;

/// num_teric quantity
template <typename Dimensions>
using num_tQ = boost::units::quantity<Dimensions, num_t>;

using dimensionless       = num_tQ<busi::dimensionless>;        ///< [-]
using length              = num_tQ<busi::length>;               ///< [m]
using area                = num_tQ<busi::area>;                 ///< [m2]
using volume              = num_tQ<busi::volume>;               ///< [m3]
using energy              = num_tQ<busi::energy>;               ///< [J]
using time                = num_tQ<busi::time>;                 ///< [s]
using mass                = num_tQ<busi::mass>;                 ///< [kg]
using temperature         = num_tQ<busi::temperature>;          ///< [K]
using density             = num_tQ<busi::mass_density>;         ///< [kg/m3]
using velocity            = num_tQ<busi::velocity>;             ///< [m/s]
using pressure            = num_tQ<busi::pressure>;             ///< [kg/(ms2)]
using energy_density      = num_tQ<busi::energy_density>;       ///< [J/m3]
using specific_energy     = num_tQ<busi::specific_energy>;      ///< [J/kg]
using dynamic_viscosity   = num_tQ<busi::dynamic_viscosity>;    ///< [kg/(m s)]
using thermal_diffusivity = num_tQ<busi::thermal_diffusivity>;  ///< [m^2/s]
using specific_gas_constant
 = num_tQ<busi::specific_gas_constant>;  ///< [J/(kg K)]

}  // namespace quantity

/// Unit types
namespace unit {

namespace bu = boost::units;

using bu::pow;
using bu::sqrt;

const constexpr auto K  = bu::si::kelvin;
const constexpr auto J  = bu::si::joule;
const constexpr auto kg = bu::si::kilogram;
const constexpr auto m3 = bu::si::cubic_meter;
const constexpr auto m2 = bu::si::square_meter;
const constexpr auto m  = bu::si::meter;

/// Memory units
using byte
 = boost::units::quantity<boost::units::byte_base_unit::unit_type, num_t>;
using mega_byte
 = boost::units::quantity<boost::units::megabyte_base_unit::unit_type, num_t>;
using giga_byte
 = boost::units::quantity<boost::units::gigabyte_base_unit::unit_type, num_t>;
static const constexpr auto bytes     = boost::units::bytes;
static const constexpr auto megabytes = boost::units::megabytes;
static const constexpr auto gigabytes = boost::units::gigabytes;

/// Memory unit utilities
namespace memory {

template <typename T> unit::mega_byte amount(T* from, T* to) {
  char* from_c = reinterpret_cast<char*>(from);
  char* to_c   = reinterpret_cast<char*>(to);
  return unit::mega_byte{num_t{to_c - from_c} * unit::bytes};
}

template <typename T> unit::mega_byte amount(T* from, Ind size) {
  T* to = from + size;
  return amount(from, to);
}

template <typename Container> unit::mega_byte amount(Container&& c) {
  return amount(c.data(), c.size());
}

}  // namespace memory
}  // namespace unit
}  // namespace hm3
#endif
