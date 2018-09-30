cmake_minimum_required(VERSION 3.9)

project(cppBUDDY)

############################
###get external projects
############################
include(GNUInstallDirs)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include(cmake/jsoncpp.cmake)
include(cmake/jsonrpc.cmake)
include(cmake/fmt.cmake)
include(cmake/boost.cmake)
include(cmake/cxxopts.cmake)



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
  src/core/OwnershipTransferOp.cpp
  src/core/OpReturnTx.cpp
  src/core/Block.cpp
  src/daemon/OdinDaemon.cpp)

#use c++17 as standard
set_target_properties(cppBUDDY PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF)


add_dependencies(cppBUDDY fmt-project)
add_dependencies(cppBUDDY jsonrpc-project)
add_dependencies(cppBUDDY boost-project)
add_dependencies(cppBUDDY cxxopts-project)


# make headers available
target_include_directories(cppBUDDY PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  fmt
  jsonrpc::client
  jsoncpp
  ${CXXOPTS_INCLUDE_DIR})

#link against boost and thread libs
target_link_libraries(cppBUDDY LINK_PUBLIC
  fmt
  jsonrpc::client
  jsoncpp
  ${CMAKE_THREAD_LIBS_INIT})
