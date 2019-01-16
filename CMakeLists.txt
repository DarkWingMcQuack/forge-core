cmake_minimum_required(VERSION 3.9)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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
include(cmake/utilxx.cmake)
include(cmake/cpptoml.cmake)
include(cmake/cxxopts.cmake)
include(cmake/CLI11.cmake)
include(cmake/gtest.cmake)
include(cmake/g3log.cmake)

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
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Block.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Coin.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/ReadOnlyDaemonBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/WriteOnlyDaemonBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/DaemonError.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/odin/ReadOnlyOdinDaemon.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/odin/ReadWriteOdinDaemon.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Transaction.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/EntryLookup.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/LookupManager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/env/LoggingSetup.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/env/ProgramOptions.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/wallet/ReadOnlyWallet.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/wallet/ReadWriteWallet.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/wallet/WalletError.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/rpc/LookupOnlyServer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/rpc/ReadOnlyWalletServer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/rpc/ReadWriteWalletServer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/cli/LookupOnlySubcommands.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/cli/ReadOnlySubcommands.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/cli/ReadWriteSubcommands.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/cli/CLIGlobalVariables.hpp

  PRIVATE
  src/core/Operation.cpp
  src/core/Entry.cpp
  src/core/EntryCreationOp.cpp
  src/core/EntryRenewalOp.cpp
  src/core/EntryDeletionOp.cpp
  src/core/EntryUpdateOp.cpp
  src/core/OwnershipTransferOp.cpp
  src/core/Block.cpp
  src/core/Coin.cpp
  src/daemon/ReadOnlyDaemonBase.cpp
  src/daemon/WriteOnlyDaemonBase.cpp
  src/daemon/odin/ReadOnlyOdinDaemon.cpp
  src/daemon/odin/ReadWriteOdinDaemon.cpp
  src/core/Transaction.cpp
  src/lookup/EntryLookup.cpp
  src/lookup/LookupManager.cpp
  src/env/LoggingSetup.cpp
  src/env/ProgramOptions.cpp
  src/wallet/ReadOnlyWallet.cpp
  src/wallet/ReadWriteWallet.cpp
  src/rpc/LookupOnlyServer.cpp
  src/rpc/ReadOnlyWalletServer.cpp
  src/rpc/ReadWriteWalletServer.cpp
  src/cli/LookupOnlySubcommands.cpp
  src/cli/ReadOnlySubcommands.cpp
  src/cli/ReadWriteSubcommands.cpp
  )

# make headers available
target_include_directories(buddy-src PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${UTILXX_INCLUDE_DIR}
  ${CPPTOML_INCLUDE_DIR}
  ${CXXOPTS_INCLUDE_DIR}
  ${CLI11_INCLUDE_DIR}
  fmt
  g3logger
  jsonrpc::client
  jsoncpp)

#link against libarys
target_link_libraries(buddy-src LINK_PUBLIC
  fmt
  jsonrpc::client
  jsonrpc::server
  jsoncpp
  g3logger
  stdc++fs #TODO: this is horrible to do imo
  ${CMAKE_THREAD_LIBS_INIT})

#executable buddyd
add_executable(buddyd
  src/buddyd.cpp)

#executable buddy-cli
add_executable(buddy-cli
  src/buddy-cli.cpp)


#make sure everything is build in the right order
add_dependencies(buddy-src fmt-project)
add_dependencies(buddy-src jsonrpc-project)
# add_dependencies(buddy-src boost-project)
add_dependencies(buddy-src cxxopts-project)
add_dependencies(buddy-src g3logger-project)
add_dependencies(buddy-src utilxx-project)
add_dependencies(buddy-src cpptoml-project)
add_dependencies(buddyd buddy-src)
add_dependencies(buddy-cli buddy-src)


# make headers available in buddyd.cpp
target_include_directories(buddyd PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  fmt
  ${CXXOPTS_INCLUDE_DIR})

#link buddyd.cpp against fmt and thread lib
target_link_libraries(buddyd LINK_PUBLIC
  buddy-src
  fmt
  jsonrpc::server
  ${CMAKE_THREAD_LIBS_INIT})

# make headers available in buddy-cli.cpp
target_include_directories(buddy-cli PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CLI11_INCLUDE_DIR}
  fmt)

#link buddy-cli.cpp against fmt and thread lib
target_link_libraries(buddy-cli LINK_PUBLIC
  buddy-src
  fmt
  ${CMAKE_THREAD_LIBS_INIT})



if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
