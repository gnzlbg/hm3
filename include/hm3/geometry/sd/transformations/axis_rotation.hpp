

// /// Rotates the signed-distance field to be evaluated on the point \p by around
// /// the axis \p axis by \p angle radians.
// ///
// /// The rotation trasformation is: x_transformed = A * x;
// /// The inverse rotation is: A^-1 * x_transformed =  x;
// template <typename P, typename V>
// P rotate(P p, num_t angle, V axis) {
//   static_assert(Point<P>{});
//   static_assert(Vector<V>{});
//   static_assert(ad_v<P> == ad_v<V>);
//   constexpr dim_t ad = ad_v<P>;
//   static_assert(ad == 3);
//   return P(math::inverse_rotation_matrix_3d(angle, axis()) * p());
// }
