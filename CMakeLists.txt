cmake_minimum_required(VERSION 3.9)

project(cppBUDDY)

#debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g3 -ggdb")

#release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -march=native -fstack-protector-strong")

############################
###CCache
############################
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

#build compilation database, needed for autocompletion in emacs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#link static
set(Boost_USE_STATIC_LIBS ON)

# use boose program options
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

# needed for multithreading
find_package(Threads)

#sources
add_executable(cppBUDDY
  main.cpp
  src/core/Operation.cpp
  src/core/Entry.cpp
  src/core/EntryCreationOp.cpp
  src/core/EntryRenewalOp.cpp
  src/core/EntryDeletionOp.cpp
  src/core/EntryUpdateOp.cpp
  src/core/OwnershipTransferOp.cpp)

#use c++17 as standard
set_target_properties(cppBUDDY PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF)


############################
###get external projects
############################
include(GNUInstallDirs)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(fmt_DIR "${cppBUDDY_SOURCE_DIR}/external/fmt")
set(fmt_INCLUDE_DIR "${fmt_DIR}/include/")
set(fmt_LIBRARY "${fmt_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}fmt${CMAKE_STATIC_LIBRARY_SUFFIX}")
ExternalProject_Add(fmt
  PREFIX ${CMAKE_BINARY_DIR}/external/fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG master
  UPDATE_COMMAND ""
  SOURCE_DIR ${CMAKE_BINARY_DIR}/external/fmt-src
  BINARY_DIR ${CMAKE_BINARY_DIR}/external/fmt-build
  CMAKE_ARGS -DFMT_TEST:BOOL=OFF
  -DFMT_INSTALL:BOOL=ON
  -DFMT_DOC:BOOL=OFF
  -DCMAKE_INSTALL_PREFIX=${fmt_DIR}
  )
#make sure fmt is downloaded and compiled before building
#cppBUDDY
add_dependencies(cppBUDDY fmt)


# make headers available
target_include_directories(cppBUDDY PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${fmt_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR})

#link against boost and thread libs
target_link_libraries(cppBUDDY LINK_PUBLIC
  ${Boost_LIBRARIES}
  ${fmt_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT})
