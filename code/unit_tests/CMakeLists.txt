cmake_minimum_required(VERSION 2.8)
project(unit_tests)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/synchconsole_test)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/exception_test)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vm_test)

add_custom_target(validate COMMAND synchconsole_test
                           COMMAND exception_test
                           COMMAND vm_test)
