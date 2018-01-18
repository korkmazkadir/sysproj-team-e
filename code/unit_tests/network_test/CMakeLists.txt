cmake_minimum_required(VERSION 2.8)
project(network_test)


include(${CMAKE_CURRENT_SOURCE_DIR}/../common/CMakeLists.txt)

list(REMOVE_ITEM src "${CMAKE_CURRENT_SOURCE_DIR}/../../userprog/SynchPost.cc")

set(SOURCES ${src} ${CMAKE_CURRENT_SOURCE_DIR}/network_test.cc)


add_executable(network_test ${SOURCES}) 


