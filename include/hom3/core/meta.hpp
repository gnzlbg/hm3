#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file \todo describe file
////////////////////////////////////////////////////////////////////////////////
// MPL:
#include <boost/mpl/vector.hpp>
#include <boost/mpl/zip_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/equal.hpp>
// Fusion:
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/container/generation/make_map.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/algorithm/auxiliary/copy.hpp>
// MPL-Fusion:
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/convert.hpp>
// Adapt structs to random, associative, fusion sequences
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace hom3 {

namespace mpl {

using namespace boost::mpl;

}  // namespace mpl

/// Fusion utilities
namespace fusion {

using namespace boost::fusion;

template <class Output, class Predicate> struct map_mutator {
  map_mutator(Output& map) : map_out(map) {}
  template <class InputPair> void operator()(const InputPair& inputPair) const {
    Predicate()(inputPair, map_out);
  }
  Output& map_out;
};

struct assign {
  template <class Input, class Output>
  void operator()(const Input& i, Output& o) const {
    fusion::at_key<typename Input::first_type>(o) = i.second;
  }
};

struct assign_dereference {
  template <class Input, class Output>
  void operator()(const Input& i, Output& o) const {
    fusion::at_key<typename Input::first_type>(o) = *i.second;
  }
};

template <class Output> using map_assigner = map_mutator<Output, assign>;
template <class Output>
using map_dereferencer = map_mutator<Output, assign_dereference>;

}  // namespace fusion
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////

namespace boost {
namespace fusion {

/// Swap function for fusion types
template <class T> inline void swap(T&& lhs, T&& rhs) noexcept {
  using std::swap;
  std::remove_reference_t<T> tmp = lhs;
  lhs = std::move(rhs);
  rhs = std::move(tmp);
}

}  // namespace fusion
}  // namespace boost
////////////////////////////////////////////////////////////////////////////////
