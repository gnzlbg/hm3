#pragma once
/// \file
///
/// Rotation matrices.
#include <hm3/ext/eigen.hpp>
#include <hm3/math/constants.hpp>
#include <hm3/types.hpp>

namespace hm3::math {

/// 2D rotation matrix (angle in radians).
Eigen::Rotation2D<num_t> rotation_matrix_2d(num_t angle) {
  return Eigen::Rotation2D<num_t>(angle);
}

/// 2D inverse rotation matrix (angle in radians).
Eigen::Rotation2D<num_t> inverse_rotation_matrix_2d(num_t angle) {
  return rotation_matrix_2d(angle).inverse();
}

/// 3D rotation matrix around the axis (angle in radians).
Eigen::AngleAxis<num_t> rotation_matrix_3d(num_t angle,
                                           Eigen::Matrix<num_t, 3, 1> axis) {
  return Eigen::AngleAxis<num_t>(angle, axis);
}

/// 3D rotation matrix around the axis (angle in radians).
Eigen::AngleAxis<num_t> inverse_rotation_matrix_3d(
 num_t angle, Eigen::Matrix<num_t, 3, 1> axis) {
  return rotation_matrix_3d(angle, axis).inverse();
}

/// 3D rotation matrix around the x-axis (angle in radians).
Eigen::AngleAxis<num_t> rotation_matrix_3d_x(num_t angle) {
  return Eigen::AngleAxis<num_t>(angle, Eigen::Matrix<num_t, 3, 1>::UnitX());
}

/// 3D rotation matrix around the y-axis (angle in radians).
Eigen::AngleAxis<num_t> rotation_matrix_3d_y(num_t angle) {
  return Eigen::AngleAxis<num_t>(angle, Eigen::Matrix<num_t, 3, 1>::UnitY());
}

/// 3D rotation matrix around the z-axis (angle in radians).
Eigen::AngleAxis<num_t> rotation_matrix_3d_z(num_t angle) {
  return Eigen::AngleAxis<num_t>(angle, Eigen::Matrix<num_t, 3, 1>::UnitZ());
}

/// 3D inverse rotation matrix around the x-axis (angle in radians).
Eigen::AngleAxis<num_t> inverse_rotation_matrix_3d_x(num_t angle) {
  return rotation_matrix_3d_x(angle).inverse();
}

/// 3D inverse rotation matrix around the y-axis (angle in radians).
Eigen::AngleAxis<num_t> inverse_rotation_matrix_3d_y(num_t angle) {
  return rotation_matrix_3d_y(angle).inverse();
}

/// 3D inverse rotation matrix around the z-axis (angle in radians).
Eigen::AngleAxis<num_t> inverse_rotation_matrix_3d_z(num_t angle) {
  return rotation_matrix_3d_z(angle).inverse();
}

}  // namespace hm3::math
