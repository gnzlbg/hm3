#include <hm3/grid/structured/tile/cell/indexed_coordinate.hpp>

namespace hm3 {

template <typename I, typename X>
void coordinate_tests(I index, suint_t length, const X x) {
  using namespace grid::structured;
  {
    X a;
    CHECK(!a);
  }
  X y(index);
  CHECK(y.idx() == index);
  CHECK(I(y) == index);
  CHECK(x == y);
  CHECK(!(x != y));
  CHECK(x <= y);
  CHECK(x >= y);

  if (index < length - 1) {
    X z(++index);
    CHECK(x != z);
    CHECK(!(x == z));
    CHECK(x < z);
    CHECK(x <= z);
    CHECK(z > x);
    CHECK(z >= x);
  }

  // Single offset +1
  for (auto&& d : X::dimensions()) {
    const auto w = x.offset(d, 1);
    if (x[d] < length - 1) {
      CHECK(w);
      CHECK(tile::cell::distance(x, w) == 1.);
      CHECK(tile::cell::distance(x, w) == tile::cell::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Single offset -1
  for (auto&& d : X::dimensions()) {
    auto w = x.offset(d, -1);
    if (x[d] > 0) {
      CHECK(w);
      CHECK(tile::cell::distance(x, w) == 1.);
      CHECK(tile::cell::distance(x, w) == tile::cell::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Array offset +1
  for (auto&& d : X::dimensions()) {
    hm3::array<sint_t, X::dimension()> o;
    o.fill(0);
    o[d]   = 1;
    auto w = x.offset(o);
    if (x[d] < length - 1) {
      CHECK(w);
      CHECK(tile::cell::distance(x, w) == 1.);
      CHECK(tile::cell::distance(x, w) == tile::cell::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }

  // Array offset -1
  for (auto&& d : X::dimensions()) {
    hm3::array<sint_t, X::dimension()> o;
    o.fill(0);
    o[d]   = -1;
    auto w = x.offset(o);
    if (x[d] > 0) {
      CHECK(w);
      CHECK(tile::cell::distance(x, w) == 1.);
      CHECK(tile::cell::distance(x, w) == tile::cell::distance(w, x));
      CHECK(distance_square(x, w) == suint_t{1});
      CHECK(distance_square(x, w) == distance_square(w, x));
    } else {
      CHECK(!w);
    }
  }
}

}  // namespace hm3
