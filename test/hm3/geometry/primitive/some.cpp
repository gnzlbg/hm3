#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <typename Some, typename Value>
auto check_some(Some& a, Value v) {
  a = Some(v);
  CHECK(visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(VertexAccess<Value>{}) {
         if
           constexpr(VertexAccess<T>{}) {
             test::check_equal(vertices(i), vertices(v));
             return true;
           }
         else {
           return false;
         }
       }
     return true;
   },
   a));

  CHECK(visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(EdgeAccess<Value>{}) {
         if
           constexpr(EdgeAccess<T>{}) {
             test::check_equal(edges(i), edges(v));
             return true;
           }
         else {
           return false;
         }
       }
     return true;
   },
   a));
};

int main() {
  {  // 1D
    static constexpr dim_t ad = 1;
    using p_t                 = point<ad>;
    using b_t                 = box<ad>;
    using a_t                 = aabb<ad>;
    using s_t                 = segment<ad>;
    using r_t                 = ray<ad>;
    using l_t                 = line<ad>;
    using v_t                 = vec<ad>;
    using some                = some<ad>;

    static_assert(!Segment<p_t>{});
    static_assert(!EdgeAccess<p_t>{});

    auto p0 = p_t::constant(0.);
    auto p1 = p_t::constant(1.);
    auto p2 = p_t::constant(2.);
    auto d0 = v_t::constant(1.).normalized();

    auto b0 = b_t(p2, .5);
    auto a0 = a_t(p1, p2);
    auto s0 = s_t(p1, p2);
    auto r0 = r_t(p1, d0);
    auto l0 = l_t(p1, d0);

    some a;
    check_some(a, p0);
    check_some(a, b0);
    check_some(a, a0);
    check_some(a, s0);
    // check_some(a, r0, r0.as_segment());
    // check_some(a, l0, l0.as_segment());
  }

  {  // 2D
    static constexpr dim_t ad = 2;
    using p_t                 = point<ad>;
    using b_t                 = box<ad>;
    using a_t                 = aabb<ad>;
    using s_t                 = segment<ad>;
    using r_t                 = ray<ad>;
    using l_t                 = line<ad>;
    using v_t                 = vec<ad>;
    using bp_t                = polygon<ad>;
    using fp_t                = fixed_polygon<ad, 5>;
    using some                = some<ad>;

    auto p0 = p_t::constant(0.);
    auto p1 = p_t::constant(1.);
    auto p2 = p_t::constant(2.);
    auto p3 = p_t::constant(3.);
    auto p4 = p_t::constant(4.);

    auto d0 = v_t::constant(1.).normalized();

    auto vxs_bp = {p0, p1, p2};
    auto vxs_fp = {p0, p1, p2, p3, p4};

    auto b0 = b_t(p2, .5);
    auto a0 = a_t(p1, p2);
    auto s0 = s_t(p1, p2);
    auto r0 = r_t(p1, d0);
    auto l0 = l_t(p1, d0);
    auto bp = bp_t(make_segment_range(vxs_bp));
    auto fp = fp_t(make_segment_range(vxs_fp));

    some a;
    check_some(a, p0);
    check_some(a, b0);
    check_some(a, a0);
    check_some(a, s0);
    // check_some(a, r0, r0.as_segment());
    // check_some(a, l0, l0.as_segment());
    check_some(a, bp);
    check_some(a, fp);
  }

  {  // 3D
    static constexpr dim_t ad = 3;
    using p_t                 = point<ad>;
    using b_t                 = box<ad>;
     using a_t                 = aabb<ad>;
    using s_t  = segment<ad>;
    using r_t  = ray<ad>;
    using l_t  = line<ad>;
    using v_t  = vec<ad>;
    using bp_t = polygon<ad>;
    using fp_t = fixed_polygon<ad, 5>;
    using some = some<ad>;

    auto p0 = p_t::constant(0.);
    auto p1 = p_t::constant(1.);
    auto p2 = p_t::constant(2.);
    auto p3 = p_t::constant(3.);
    auto p4 = p_t::constant(4.);

    auto d0 = v_t::constant(1.).normalized();

    auto b0 = b_t(p2, .5);
    auto a0 = a_t(p1, p2);
    auto s0 = s_t(p1, p2);
    auto r0 = r_t(p1, d0);
    auto l0 = l_t(p1, d0);

    auto vxs_bp = {p0, p1, p2};
    auto vxs_fp = {p0, p1, p2, p3, p4};
    auto bp     = bp_t(make_segment_range(vxs_bp));
    auto fp     = fp_t(make_segment_range(vxs_fp));

    some a;
    check_some(a, p0);
    check_some(a, b0);
    check_some(a, a0);
    check_some(a, s0);
    // check_some(a, r0, r0.as_segment());
    // check_some(a, l0, l0.as_segment());
    check_some(a, bp);
    check_some(a, fp);
  }

  return 0;
}
