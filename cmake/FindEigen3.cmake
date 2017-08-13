# Find the Eigen3 include directory
# The following variables are set if Eigen3 is found.
#  Eigen3_FOUND        - True when the Eigen3 include directory is found.
#  Eigen3_INCLUDE_DIR  - The path to where the Eigen3 include files are.
# If Eigen3 is not found, Eigen3_FOUND is set to false.

find_package(PkgConfig)

#if(NOT EXISTS "${Eigen3_INCLUDE_DIR}")
#  find_path(Eigen3_INCLUDE_DIR
#    NAMES Eigen
#    DOC "Eigen3 library header files"
#    )
#endif()

#if(EXISTS "${Eigen3_INCLUDE_DIR}")
#  include(FindPackageHandleStandardArgs)
#  mark_as_advanced(Eigen3_INCLUDE_DIR)
#else()
  include(ExternalProject)
  ExternalProject_Add(Eigen3
    HG_REPOSITORY https://bitbucket.org/eigen/eigen/
    TIMEOUT 5
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    CONFIGURE_COMMAND "" # Disable configure step
    BUILD_COMMAND "" # Disable build step
    INSTALL_COMMAND "" # Disable install step
    UPDATE_COMMAND "" # Disable update step: clones the project only once
    )
  
  # Specify include dir
  ExternalProject_Get_Property(Eigen3 source_dir)
  set(Eigen3_INCLUDE_DIR ${source_dir})
#endif()

if(EXISTS "${Eigen3_INCLUDE_DIR}")
  set(Eigen3_FOUND 1)
else()
  set(Eigen3_FOUND 0)
endif()
