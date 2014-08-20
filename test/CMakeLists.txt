################################################################################
# \file \brief Unit Tests
################################################################################
enable_testing()
add_custom_target(tests
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "test.+"
  COMMENT "Build and run all the unit tests.")

function(h3_test name)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" ";" FLS "${CMAKE_CURRENT_SOURCE_DIR}")
  list (GET FLS 1 FOLDER)
  string(REGEX REPLACE "/" "." BASE_NAME "${FOLDER}")
  set(TEST_NAME ${BASE_NAME}.${name})
  add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${name}.cpp)
  target_link_libraries(${TEST_NAME} ${H3_LIBS})
  add_test(${TEST_NAME} ${TEST_NAME})
  add_dependencies(tests ${TEST_NAME})
endfunction(h3_test)

function(h3_mpi_test name no_proc)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" ";" FLS "${CMAKE_CURRENT_SOURCE_DIR}")
  list (GET FLS 1 FOLDER)
  string(REGEX REPLACE "/" "." BASE_NAME "${FOLDER}")
  set(TEST_NAME ${BASE_NAME}.${name})
  add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${name}.cpp)
  target_link_libraries(${TEST_NAME} ${H3_LIBS})
  set(test_parameters -np ${no_proc} "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}")
  add_test(${TEST_NAME} mpirun ${test_parameters})
  add_dependencies(tests ${TEST_NAME})
endfunction(h3_mpi_test)

add_subdirectory(core)
add_subdirectory(grid)
add_subdirectory(io)
