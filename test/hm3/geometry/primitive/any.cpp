#include <hm3/geometry/primitive/any.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <typename Any, typename Value, typename ValueShould>
auto check_any(Any& a, Value v, ValueShould vs) {
  using VT = uncvref_t<ValueShould>;
  a        = uncvref_t<Any>(v);
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, VT>{}) { CHECK(i == vs); }
     else {
       CHECK(false);
     }
   },
   a);
};

int main() {
  {  // 1D
    static constexpr dim_t nd = 1;
    using p_t                 = geometry::point<nd>;
    using b_t                 = geometry::box<nd>;
    using a_t                 = geometry::aabb<nd>;
    using s_t                 = geometry::segment<nd>;
    using r_t                 = geometry::ray<nd>;
    using l_t                 = geometry::line<nd>;
    using v_t                 = geometry::vec<nd>;
    using any                 = geometry::any<nd>;

    auto p0 = p_t::constant(0.);
    auto p1 = p_t::constant(1.);
    auto p2 = p_t::constant(2.);
    auto d0 = v_t::constant(1.).normalized();

    auto b0 = b_t(p2, .5);
    auto a0 = a_t(p1, p2);
    auto s0 = s_t(p1, p2);
    auto r0 = r_t(p1, d0);
    auto l0 = l_t(p1, d0);

    any a;
    check_any(a, p0, p0);
    check_any(a, b0, b0);
    check_any(a, a0, a0);
    check_any(a, s0, s0);
    check_any(a, r0, r0.as_segment());
    check_any(a, l0, l0.as_segment());
  }

  {  // 2D
    static constexpr dim_t nd = 2;
    using p_t                 = geometry::point<nd>;
    using b_t                 = geometry::box<nd>;
    using a_t                 = geometry::aabb<nd>;
    using s_t                 = geometry::segment<nd>;
    using r_t                 = geometry::ray<nd>;
    using l_t                 = geometry::line<nd>;
    using v_t                 = geometry::vec<nd>;
    using bp_t                = geometry::bounded_polygon<nd, 3>;
    using sp_t                = geometry::small_polygon<nd, 4>;
    using fp_t                = geometry::fixed_polygon<nd, 5>;
    using any                 = geometry::any<nd>;

    using any_pt = geometry::any_primitive::polygon_t<nd>;

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
    auto bp = bp_t({p0, p1, p2});
    auto sp = sp_t({p0, p1, p2, p3});
    auto fp = fp_t{{{p0, p1, p2, p3, p4}}};

    any a;
    check_any(a, p0, p0);
    check_any(a, b0, b0);
    check_any(a, a0, a0);
    check_any(a, s0, s0);
    check_any(a, r0, r0.as_segment());
    check_any(a, l0, l0.as_segment());
    check_any(a, sp, any_pt(sp));
    check_any(a, bp, any_pt(vertices(bp)));
    check_any(a, fp, any_pt(vertices(fp)));
  }

  {  // 3D
    static constexpr dim_t nd = 3;
    using p_t                 = geometry::point<nd>;
    using b_t                 = geometry::box<nd>;
    using a_t                 = geometry::aabb<nd>;
    using s_t                 = geometry::segment<nd>;
    using r_t                 = geometry::ray<nd>;
    using l_t                 = geometry::line<nd>;
    using v_t                 = geometry::vec<nd>;
    using bp_t                = geometry::bounded_polygon<nd, 3>;
    using sp_t                = geometry::small_polygon<nd, 4>;
    using fp_t                = geometry::fixed_polygon<nd, 5>;
    using any                 = geometry::any<nd>;

    using any_pt = geometry::any_primitive::polygon_t<nd>;

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
    auto bp = bp_t({p0, p1, p2});
    auto sp = sp_t({p0, p1, p2, p3});
    auto fp = fp_t{{{p0, p1, p2, p3, p4}}};

    any a;
    check_any(a, p0, p0);
    check_any(a, b0, b0);
    check_any(a, a0, a0);
    check_any(a, s0, s0);
    check_any(a, r0, r0.as_segment());
    check_any(a, l0, l0.as_segment());
    check_any(a, sp, any_pt(sp));
    check_any(a, bp, any_pt(vertices(bp)));
    check_any(a, fp, any_pt(vertices(fp)));
  }

  return 0;
}
