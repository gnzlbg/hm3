#pragma once
/// \file variant.hpp
#include <hm3/utility/range-v3.hpp>

namespace hm3 {

namespace variant_detail {
using ranges::v3::tagged_variant;
using ranges::v3::get;
using ranges::v3::set;
}  // namespace variant_detail

template <class... Ts> using variant = variant_detail::tagged_variant<Ts...>;
using variant_detail::get;
using variant_detail::set;

}  // namespace hm3
