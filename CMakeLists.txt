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

option(USE_CLANG "build application with clang" OFF) # OFF is the default
if(USE_CLANG)
  SET(CMAKE_C_COMPILER    "clang")
  SET(CMAKE_CXX_COMPILER  "clang++")
  SET(CMAKE_AR            "llvm-ar")
  SET(CMAKE_NM            "llvm-nm")
  SET(CMAKE_OBJDUMP       "llvm-objdump")
  SET(CMAKE_RANLIB        "llvm-ranlib")
endif(USE_CLANG)

option(GENERATE_COMPILE_DATABASE "generate a compile_commands.json" OFF)
if(GENERATE_COMPILE_DATABASE)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif(GENERATE_COMPILE_DATABASE)



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
  include/core/Operation.hpp
  src/core/Operation.cpp
  src/core/Entry.cpp
  include/core/Entry.hpp
  src/core/EntryCreationOp.cpp
  include/core/EntryCreationOp.hpp
  src/core/EntryRenewalOp.cpp
  include/core/EntryRenewalOp.hpp
  src/core/EntryDeletionOp.cpp
  include/core/EntryDeletionOp.hpp
  src/core/EntryUpdateOp.cpp
  include/core/EntryUpdateOp.hpp
  src/core/OwnershipTransferOp.cpp
  include/core/OwnershipTransferOp.hpp
  src/core/OpReturnTx.cpp
  include/core/OpReturnTx.hpp
  src/core/Block.cpp
  include/core/Block.hpp
  include/daemon/Coin.hpp
  src/daemon/DaemonBase.cpp
  include/daemon/DaemonBase.hpp
  src/daemon/OdinDaemon.cpp
  include/daemon/OdinDaemon.hpp
  include/core/Transaction.hpp
  src/core/Transaction.cpp)

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
