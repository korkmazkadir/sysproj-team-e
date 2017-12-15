cmake_minimum_required(VERSION 2.8)
project(synchconsole_test)


include(${CMAKE_CURRENT_SOURCE_DIR}/../common/CMakeLists.txt)
list(REMOVE_ITEM src "${CMAKE_CURRENT_SOURCE_DIR}/../../userprog/synchconsole.cc")
set(SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/synchconsole_test.cc ${src})

add_executable(synchconsole_test ${SOURCES}) 


