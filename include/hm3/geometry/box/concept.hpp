
template <typename T> struct is_box : std::false_type {};

struct square : rc::refines<Rectangle> {
  template <typename T>
  auto requires_(T&& t) -> decltype(rc::is_true(is_b<rc::uncvref_t<T>>{}),
                                    rc::valid_expr(length(t)));
};

using Square = square;

template <typename T>
using Square = concepts::rc::models<concepts::Square, ranges::uncvref_t<T>>;

namespace concepts {

/// Boxs are not only rectangles but also boxs:
// template <dim_t Nd>
// struct is_box<::hm3::geometry::box<Nd>> : std::true_type {};

}  // namespace concepts
