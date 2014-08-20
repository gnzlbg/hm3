cmake_minimum_required (VERSION 2.8.7)

add_custom_target(vis)

if (HM3_ENABLE_PARAVIEW_PLUGINS)
  add_subdirectory("paraview")
endif()
