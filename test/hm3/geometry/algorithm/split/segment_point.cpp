#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/geometry/test/data_type.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Nd>  //
void test_segment_point_split() {
  using p_t = point<Nd>;
  using s_t = segment<Nd>;

  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(1.5);

  auto s  = s_t(p1, p2);
  auto r1 = s_t(p1, p4);
  auto r2 = s_t(p4, p2);

  auto s0 = split(s, p0);
  auto s1 = split(s, p1);
  auto s2 = split(s, p2);
  auto s3 = split(s, p3);
  auto s4 = split(s, p4);

  CHECK(s0.empty());
  CHECK(s1.size() == 1);
  CHECK(s1[0] == s);
  CHECK(s2.size() == 1);
  CHECK(s2[0] == s);
  CHECK(s3.empty());
  CHECK(s4.size() == 2);
  CHECK(s4[0] == r1);
  CHECK(s4[1] == r2);
}

int main() {
  test_segment_point_split<1>();
  test_segment_point_split<2>();
  test_segment_point_split<3>();

  {  // preserving edge-data-type and primitive-data-type
    static constexpr dim_t ad = 1;

    using p_t = point<ad, data_type>;
    using s_t = segment<ad, p_t, data_type>;
    p_t pL{0.};
    p_t pR{1.};
    p_t pM{.5};
    data(pL) = data_type{2};
    data(pR) = data_type{4};
    data(pM) = data_type{7};

    s_t s{pL, pR};
    data(s) = data_type{33};
    {  // test split on middle into two segments
      auto r = split(s, pM);
      CHECK(r.size() == 2_u);

      CHECK(r[0] == s_t{pL, pM});
      CHECK(r[1] == s_t{pM, pR});
      CHECK(data(r[0].x(0)) == data(pL));
      CHECK(data(r[0].x(1)) == data(pM));
      CHECK(data(r[0]) == data_type{33});
      CHECK(data(r[1].x(0)) == data(pM));
      CHECK(data(r[1].x(1)) == data(pR));
      CHECK(data(r[1]) == data_type{33});
      CHECK(data(pL) == data_type{2});
      CHECK(data(pR) == data_type{4});
      CHECK(data(pM) == data_type{7});
    }
    {  // test split on boundary, returns the original segment with original
       // data
      CHECK(data(pL) != data_type{3});
      auto pL_  = pL;
      data(pL_) = data_type{3};
      auto r    = split(s, pL_);
      CHECK(r.size() == 1_u);

      CHECK(r[0] == s);
      CHECK(data(r[0]) == data(s));
      CHECK(data(r[0].x(0)) == data(pL));
      CHECK(data(r[0].x(1)) == data(pR));
    }
  }

  return test::result();
}
