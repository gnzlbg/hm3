# Find the hm3_resources
include(ExternalProject)
ExternalProject_Add(hm3_resources
  DOWNLOAD_COMMAND git clone --depth 1 https://github.com/gnzlbg/hm3_resources.git
  DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}"
  TIMEOUT 5
  CMAKE_ARGS ""
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  CONFIGURE_COMMAND "" # Disable configure step
  BUILD_COMMAND "" # Disable build step
  INSTALL_COMMAND "" # Disable install step
  UPDATE_COMMAND "" # Disable update step: clones the project only once
  )
