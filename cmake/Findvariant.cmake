# Find the variant include directory
# The following variables are set if variant is found.
#  variant_FOUND        - True when the variant include directory is found.
#  variant_INCLUDE_DIR  - The path to where the variant include files are.
# If variant is not found, variant_FOUND is set to false.

find_package(PkgConfig)

if(NOT EXISTS "${variant_INCLUDE_DIR}")
  find_path(variant_INCLUDE_DIR
    NAMES variant.hpp
    DOC "variant library header files"
    )
endif()

if(EXISTS "${variant_INCLUDE_DIR}")
  include(FindPackageHandleStandardArgs)
  mark_as_advanced(variant_INCLUDE_DIR)
else()
  include(ExternalProject)
  ExternalProject_Add(variant
    DOWNLOAD_COMMAND git clone --depth 1 https://github.com/mpark/variant.git
    TIMEOUT 5
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    CONFIGURE_COMMAND "" # Disable configure step
    BUILD_COMMAND "" # Disable build step
    INSTALL_COMMAND "" # Disable install step
    UPDATE_COMMAND "" # Disable update step: clones the project only once
    )
  
  # Specify include dir
  ExternalProject_Get_Property(variant source_dir)
  set(variant_INCLUDE_DIR ${source_dir}/include)
endif()

if(EXISTS "${variant_INCLUDE_DIR}")
  set(variant_FOUND 1)
else()
  set(variant_FOUND 0)
endif()
