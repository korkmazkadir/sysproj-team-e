cmake_minimum_required(VERSION 2.8)
project(filesys_test)


include(${CMAKE_CURRENT_SOURCE_DIR}/../common/CMakeLists.txt)

set(SOURCES ${src} ${CMAKE_CURRENT_SOURCE_DIR}/filesys_test.cc)


add_executable(filesys_test ${SOURCES}) 


