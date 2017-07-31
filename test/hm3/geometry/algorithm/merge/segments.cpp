#include <hm3/geometry/algorithm/merge.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/geometry/test/data_type.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  static constexpr dim_t ad = 1;
  {  // without data:
    using p_t = point<ad>;
    using s_t = segment<ad, p_t>;

    p_t p0 = {0.};
    p_t p1 = {1.};
    p_t p2 = {2.};
    p_t p3 = {3.};

    s_t s0 = {p0, p1};
    s_t s1 = {p1, p2};
    s_t s2 = {p2, p3};
    s_t sr = {p0, p2};

    auto r01 = merge(s0, s1);
    CHECK(r01);
    CHECK(r01.value() == sr);
    auto f = merge(s0, s2);
    CHECK(!f);
  }

  {  // with segment data:
    using p_t = point<ad>;
    using s_t = segment<ad, p_t, data_type>;

    p_t p0 = {0.};
    p_t p1 = {1.};
    p_t p2 = {2.};
    p_t p3 = {3.};

    s_t s0 = {p0, p1};
    s_t s1 = {p1, p2};
    s_t s2 = {p2, p3};
    s_t sr = {p0, p2};

    // data is equal: can merge
    CHECK(data(s0) == data(s1));
    {
      auto r01 = merge(s0, s1);
      CHECK(r01);
      CHECK(r01.value() == sr);
      auto f = merge(s0, s2);
      CHECK(!f);
    }
    // data is different: cannot merge
    data(s0) = data_type{3};
    CHECK(data(s0) != data(s1));
    {
      auto r01 = merge(s0, s1);
      CHECK(!r01);
    }
  }

  {  // with point data:
    using p_t = point<ad, data_type>;
    using s_t = segment<ad, p_t>;

    p_t p0 = {0.};
    p_t p1 = {1.};
    p_t p2 = {2.};
    p_t p3 = {3.};

    {
      s_t s0 = {p0, p1};
      s_t s1 = {p1, p2};
      s_t s2 = {p2, p3};
      s_t sr = {p0, p2};

      // data of all points is equal: can merge
      CHECK(data(p0) == data(p1));
      CHECK(data(p1) == data(p2));
      {
        auto r01 = merge(s0, s1);
        CHECK(r01);
        CHECK(r01.value() == sr);
        auto f = merge(s0, s2);
        CHECK(!f);
      }
      // data of p1 != (p0, p2), but equal in both segments, cannot merge:
      data(s1.x(1)) = data_type{3};
      data(s2.x(0)) = data_type{3};
      CHECK(data(s1.x(0)) != data(s1.x(1)));
      CHECK(data(s2.x(0)) != data(s2.x(1)));
      CHECK(data(s1.x(1)) == data(s2.x(0)));
      {
        auto r01 = merge(s0, s1);
        CHECK(!r01);
      }
      // data of p1 != (p0, p2), different in each segment, cannot merge:
      data(s2.x(0)) = data_type{4};
      CHECK(data(s1.x(0)) != data(s1.x(1)));
      CHECK(data(s2.x(0)) != data(s2.x(1)));
      CHECK(data(s1.x(1)) != data(s2.x(0)));
      {
        auto r01 = merge(s0, s1);
        CHECK(!r01);
      }
    }
  }

  {  // TODO: test with both point and segment data
  }

  return 0;
}
