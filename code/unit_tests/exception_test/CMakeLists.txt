cmake_minimum_required(VERSION 2.8)
project(exception_test)


include(${CMAKE_CURRENT_SOURCE_DIR}/../common/CMakeLists.txt)

list(REMOVE_ITEM src "${CMAKE_CURRENT_SOURCE_DIR}/../../userprog/exception.cc")

set(SOURCES ${src} ${CMAKE_CURRENT_SOURCE_DIR}/exception_test.cc)


add_executable(exception_test ${SOURCES}) 


