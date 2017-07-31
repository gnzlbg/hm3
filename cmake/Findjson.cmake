# Find the json include directory
# The following variables are set if json is found.
#  json_FOUND        - True when the json include directory is found.
#  json_INCLUDE_DIR  - The path to where the json include files are.
# If json is not found, json_FOUND is set to false.
find_package(PkgConfig)

if(NOT EXISTS "${json_INCLUDE_DIR}")
  find_path(json_INCLUDE_DIR
    NAMES json.hpp
    DOC "json library header files"
    )
endif()

if(EXISTS "${json_INCLUDE_DIR}")
  include(FindPackageHandleStandardArgs)
  mark_as_advanced(json_INCLUDE_DIR)
else()
  include(ExternalProject)
  ExternalProject_Add(json
    DOWNLOAD_COMMAND git clone --depth 1 https://github.com/nlohmann/json.git
    TIMEOUT 5
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    CONFIGURE_COMMAND "" # Disable configure step
    BUILD_COMMAND "" # Disable build step
    INSTALL_COMMAND "" # Disable install step
    UPDATE_COMMAND "" # Disable update step: clones the project only once
    )
  
  # Specify include dir
  ExternalProject_Get_Property(json source_dir)
  set(json_INCLUDE_DIR ${source_dir}/src)
endif()

if(EXISTS "${json_INCLUDE_DIR}")
  set(json_FOUND 1)
else()
  set(json_FOUND 0)
endif()
