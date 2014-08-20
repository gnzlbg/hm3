////////////////////////////////////////////////////////////////////////////////
/// \file Tests bit dense matrix
/// Includes:
#include <hom3/core/matrix/bit_dense.hpp>
#include <hom3/core/test.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

int main() {
  /// dynamic vector
  { dense::bit_vector<dense::dynamic> v{10}; }

  /// static vector
  { dense::bit_vector<10> v{10}; }

  /// dynamic vector with static storage
  { dense::bit_vector<dense::dynamic, Ind> v{10}; }

  /// fully dynamic row order matrix
  /// fully dynamic col order matrix
  /// dynamic row order with static rows
  /// dynamic col order with static rows
  /// dynamic row order with static cols
  /// dynamic col order with static cols
  /// static row order
  /// static col order

  return test::result();
}
////////////////////////////////////////////////////////////////////////////////
