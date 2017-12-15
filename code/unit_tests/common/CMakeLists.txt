cmake_minimum_required(VERSION 2.8)
enable_language(C ASM)
add_definitions(-std=c++11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)


file(GLOB src 
           "${CMAKE_CURRENT_SOURCE_DIR}/../../machine/*.cc"
           "${CMAKE_CURRENT_SOURCE_DIR}/../../filesys/*.cc"
           "${CMAKE_CURRENT_SOURCE_DIR}/../../network/*.cc" 
           "${CMAKE_CURRENT_SOURCE_DIR}/../../threads/*.cc" 
           "${CMAKE_CURRENT_SOURCE_DIR}/../../threads/*.S"
           "${CMAKE_CURRENT_SOURCE_DIR}/../../userprog/*.cc")

list(REMOVE_ITEM src "${CMAKE_CURRENT_SOURCE_DIR}/../../threads/main.cc")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../bin
           ${CMAKE_CURRENT_SOURCE_DIR}/../../machine 
           ${CMAKE_CURRENT_SOURCE_DIR}/../../filesys
           ${CMAKE_CURRENT_SOURCE_DIR}/../../network
           ${CMAKE_CURRENT_SOURCE_DIR}/../../threads
           ${CMAKE_CURRENT_SOURCE_DIR}/../../userprog
           ${CMAKE_CURRENT_SOURCE_DIR}/../mocks
           ${CMAKE_CURRENT_SOURCE_DIR}/../common)

add_definitions(-DUSER_PROGRAM -DFILESYS_NEEDED -DNETWORK -DLINUX -DHOST_i386 -DFILESYS -DUNIT_TESTS -Dprivate=public)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")

