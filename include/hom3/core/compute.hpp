#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Computing utilities (work-offloading)
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <type_traits>
// Thrust containers:
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
// Thrust algorithms:
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/sequence.h>
#include <thrust/replace.h>
#include <thrust/transform.h>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

namespace host {
template <class T> using vector = thrust::host_vector<T>;
}  // namespace host
namespace device {
template <class T> using vector = thrust::device_vector<T>;
}  // namespace device
namespace compute {
using namespace thrust;
}  // namespace compute

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
