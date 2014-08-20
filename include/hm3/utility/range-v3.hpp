#pragma once
/// \file
///
/// Includes the range-v3 library
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <range/v3/all.hpp>
#pragma clang diagnostic pop
#include <boost/container/vector.hpp>

/// Extensions:
namespace ranges {

template <typename T, std::size_t Capacity>
struct is_view<boost::container::static_vector<T, Capacity>> : std::false_type {
};

template <typename T, class Alloc>
struct is_view<boost::container::vector<T, Alloc>> : std::false_type {};

}  // namespace ranges
