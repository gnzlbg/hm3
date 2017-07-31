#pragma once
/// \file
///
/// Empty-base class optimization.
namespace hm3 {

template <typename T = void>
struct ebo : T {};

template <>
struct ebo<void> {};

}  // namespace hm3
