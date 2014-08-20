################################################################################
# \file \brief Paraview Plugins
################################################################################
project(HOM3_PARAVIEW_PLUGINS)

# Find ParaView build directory
h3_dep(ParaView "" "")

# Get additional macros
include(${PARAVIEW_USE_FILE})

# Add decorator
QT4_wrap_cpp(moc_srcs pqDataAvailableWidgetDecorator.h)

set(interfaces0)
set(srcs0)

add_paraview_property_widget_decorator(interfaces0 srcs0
  TYPE "dataAvailableDecorator"
  CLASS_NAME pqDataAvailableWidgetDecorator)

# Adds a Paraview Plugin
function(h3_pv_plugin name)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" ";" FLS "${CMAKE_CURRENT_SOURCE_DIR}")
  list (GET FLS 1 FOLDER)
  string(REGEX REPLACE "/" "." BASE_NAME "${FOLDER}")
  set(PLUGIN_NAME ${BASE_NAME}.${name})

  add_paraview_plugin(${PLUGIN_NAME} "v0.0"
  SERVER_MANAGER_XML "${name}_serv_conf.xml"
  GUI_INTERFACES ${interfaces0}
  SERVER_MANAGER_SOURCES ${name}.cpp
  SOURCES
    ${srcs0}
    ${moc_srcs}
    pqDataAvailableWidgetDecorator.cpp
  REQUIRED_ON_SERVER)

  add_executable(${PLUGIN_NAME} EXCLUDE_FROM_ALL ${name}.cpp)
  target_link_libraries(${PLUGIN_NAME} ${H3_LIBS})

  add_dependencies(plugins ${PLUGIN_NAME})
endfunction(h3_pv_plugin)

add_subdirectory(hom3)
