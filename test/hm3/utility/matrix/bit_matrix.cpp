/// \file
///
/// Tests bit dense matrix
#include <hm3/utility/matrix/matrix.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  /// dynamic vector
  { dense::matrix<dense::bit, dense::dynamic, 1> v(10); }

  /// static vector
  { dense::matrix<dense::bit, 10, 1> v(10); }

  /// dynamic vector with static storage
  { dense::matrix<dense::bit, dense::dynamic, 1, idx_t> v(10); }

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
