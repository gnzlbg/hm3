# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Search for packages.
#
# Behavior when the package is found or not is customized at the
# point where the package is required.

# Target for fetching packages
add_custom_target(fetch_packages)

macro(hm3_pkg name cflags lflags)
  find_package(${name} REQUIRED)
  include_directories(SYSTEM ${${name}_INCLUDE_DIR})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${name}_COMPILE_FLAGS} ${cflags}")
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${${name}_LINK_FLAGS} ${lflags}")
  add_dependencies(fetch_packages ${name})
endmacro()

# Git: parses current project commit
find_package(Git)
if (GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    OUTPUT_VARIABLE HM3_CURRENT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Doxygen
find_package(Doxygen)

# Valgrind
find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND-NOTFOUND)
  message(FATAL_ERROR "valgrind not found")
else()
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
  # set(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/test/valgrind_suppress.txt")
  include(Dart)
endif()

# Fetch from source:
hm3_pkg(range-v3 "-DRANGES_SUPPRESS_IOTA_WARNING" "")
hm3_pkg(json "" "")
hm3_pkg(spdlog "-DFMT_HEADER_ONLY" "")
hm3_pkg(Eigen3 "" "")

# MPI:
find_package(MPI REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_COMPILE_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS}")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

# Boost:
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# HM3_LIBS contains all the libraries that need to be linked to the binaries
set(HM3_LIBS ${Boost_LIBRARIES} ${MPI_LIBRARIES})

# ParaView:
if (HM3_ENABLE_PARAVIEW_PLUGINS)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
  # If ParaView found use VTK from ParaView
  set(HM3_LIBS ${HM3_LIBS} vtkalglib vtkCommonCore vtkCommonMath vtkCommonSystem
    vtkCommonComputationalGeometry vtkIOXML)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHM3_ENABLE_VTK")
endif()

# VTK: if ParaView not found but VTK enabled use systems VTK
if (HM3_ENABLE_VTK AND NOT HM3_ENABLE_PARAVIEW_PLUGINS)
  find_package(VTK REQUIRED)

  include(${VTK_USE_FILE})
  set(HM3_LIBS ${HM3_LIBS} vtkalglib vtkCommonCore vtkCommonMath vtkCommonSystem
    vtkCommonComputationalGeometry vtkIOXML)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHM3_ENABLE_VTK")
endif()

# Verbose CMake config output output:
if(HM3_VERBOSE_CONFIGURE)
  get_directory_property(COMPILE_OPTIONS_ COMPILE_OPTIONS)
  message("Verbose flags:")
  message("Build type: ${CMAKE_BUILD_TYPE}")
  message("The compiler is: ${CMAKE_CXX_COMPILER}")
  message("CXX:")
  message(" * Flags: ${CMAKE_CXX_FLAGS}")
  message(" * Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
  message(" * Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
  message(" * Compile Flags: ${CMAKE_CXX_COMPILE_FLAGS}")
  message(" * Compile options: ${COMPILE_OPTIONS_}")
  message("C:")
  message(" * Flags: ${CMAKE_C_FLAGS}")
  message(" * Compile Flags: ${CMAKE_C_COMPILE_FLAGS}")
  message("Linker flags:")
  message(" * exe:${CMAKE_EXE_LINKER_FLAGS}")
  message(" * cxx: ${CMAKE_CXX_LINK_FLAGS}")
  message(" * module: ${CMAKE_MODULE_LINKER_FLAGS}")
  get_directory_property(CMakeCompDirDefs COMPILE_DEFINITIONS)
  message("Definitions: ${CmakeCompDirDefs}")
  message("Packages:")
  message(" * Eigen3:")
  message("   - include_p: ${Eigen3_INCLUDE_DIR}")
  message(" * range-v3:")
  message("   - include_p: ${range-v3_INCLUDE_DIR}")
  message(" * json:")
  message("   - include_p: ${json_INCLUDE_DIR}")
  message(" * spdlog:")
  message("   - include_p: ${spdlog_INCLUDE_DIR}")
  message(" * Boost:")
  message("   - include_p: ${Boost_INCLUDE_DIR}")
  message("   - library_p: ${Boost_LIBRARIES}")
  message(" * MPI:")
  message("   - include_p: ${MPI_INCLUDE_PATH}")
  message("   - library_p: ${MPI_LIBRARIES}")
  if(HM3_ENABLE_PARAVIEW_PLUGINS)
    message(" * ParaView:")
    message("   - source directory: ${ParaView_DIR}")
    message("   - file: ${PARAVIEW_USE_FILE}")
  else()
    message(" * ParaView: disabled")
  endif()
  if (HM3_ENABLE_VTK)
    message(" * VTK: enabled")
    message("   - vtk.cmake: ${VTK_USE_FILE}")
    message("   - vtk.libs: ${VTK_LIBRARIES}")
  else()
    message(" * VTK: disabled")
  endif()
  message(" * HM3_LIBS: ${HM3_LIBS}")
endif()
