cmake_minimum_required(VERSION 3.9)

project(cppBUDDY)

#set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################
###get external projects
############################
include(cmake/jsoncpp.cmake)
include(cmake/jsonrpc.cmake)
include(cmake/fmt.cmake)
# include(cmake/boost.cmake)
include(cmake/cxxopts.cmake)
include(cmake/gtest.cmake)

#get options
include(cmake/options.cmake)

if(USE_CLANG)
  SET(CMAKE_C_COMPILER    "clang")
  SET(CMAKE_CXX_COMPILER  "clang++")
  SET(CMAKE_AR            "llvm-ar")
  SET(CMAKE_NM            "llvm-nm")
  SET(CMAKE_OBJDUMP       "llvm-objdump")
  SET(CMAKE_RANLIB        "llvm-ranlib")
endif(USE_CLANG)

#setup flags and ccache
include(cmake/flags.cmake)
include(cmake/ccache.cmake)


# needed for multithreading
find_package(Threads)

add_library(buddy-src "")
target_sources(buddy-src
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Operation.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Entry.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/EntryCreationOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/EntryRenewalOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/EntryDeletionOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/EntryUpdateOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/OwnershipTransferOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/OpReturnTx.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Block.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/Coin.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/DaemonBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/OdinDaemon.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Transaction.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/EntryLookup.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/LookupManager.hpp

  PRIVATE
  src/core/Operation.cpp
  src/core/Entry.cpp
  src/core/EntryCreationOp.cpp
  src/core/EntryRenewalOp.cpp
  src/core/EntryDeletionOp.cpp
  src/core/EntryUpdateOp.cpp
  src/core/OwnershipTransferOp.cpp
  src/core/OpReturnTx.cpp
  src/core/Block.cpp
  src/daemon/DaemonBase.cpp
  src/daemon/OdinDaemon.cpp
  src/core/Transaction.cpp
  src/lookup/EntryLookup.cpp
  src/lookup/LookupManager.cpp
  )

# make headers available
target_include_directories(buddy-src PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  fmt
  jsonrpc::client
  jsoncpp)

#link against libarys
target_link_libraries(buddy-src LINK_PUBLIC
  fmt
  jsonrpc::client
  jsoncpp
  ${CMAKE_THREAD_LIBS_INIT})

#executable
add_executable(cppBUDDY
  main.cpp)


#make sure everything is build in the right order
add_dependencies(buddy-src fmt-project)
add_dependencies(buddy-src jsonrpc-project)
# add_dependencies(buddy-src boost-project)
add_dependencies(buddy-src cxxopts-project)
add_dependencies(cppBUDDY buddy-src)


# make headers available in main.cpp
target_include_directories(cppBUDDY PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  fmt
  ${CXXOPTS_INCLUDE_DIR})

#link main.cpp against fmt and thread lib
target_link_libraries(cppBUDDY LINK_PUBLIC
  buddy-src
  fmt
  ${CMAKE_THREAD_LIBS_INIT})



if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
