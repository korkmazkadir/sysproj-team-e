cmake_minimum_required(VERSION 2.8)
project(vm_test)


include(${CMAKE_CURRENT_SOURCE_DIR}/../common/CMakeLists.txt)

set(SOURCES ${src} ${CMAKE_CURRENT_SOURCE_DIR}/vm_test.cc)


add_executable(vm_test ${SOURCES}) 


