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

project(cppFORGE)

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
# include(cmake/cxxopts.cmake)
include(cmake/CLI11.cmake)
include(cmake/gtest.cmake)
include(cmake/g3log.cmake)

#setup flags and ccache
include(cmake/flags.cmake)
include(cmake/ccache.cmake)


# needed for multithreading
find_package(Threads)

add_library(forge-src "")
target_sources(forge-src
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/Entry.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/EntryOperation.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryOperation.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntry.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryCreationOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryRenewalOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryDeletionOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryUpdateOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/umentry/UMEntryOwnershipTransferOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntry.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntryOperation.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntryCreationOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntryDeletionOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntryRenewalOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/uentry/UniqueEntryOwnershipTransferOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/token/UtilityToken.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/token/UtilityTokenOperation.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/token/UtilityTokenCreationOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/token/UtilityTokenDeletionOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/entrys/token/UtilityTokenOwnershipTransferOp.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Block.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Coin.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/ReadOnlyDaemonBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/WriteOnlyDaemonBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/DaemonError.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/odin/ReadOnlyOdinDaemon.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/daemon/odin/ReadWriteOdinDaemon.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/core/Transaction.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/LookupError.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/UMEntryLookup.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/UtilityTokenLookup.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lookup/UniqueEntryLookup.hpp
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
  src/entrys/Entry.cpp
  src/entrys/EntryOperation.cpp
  src/entrys/umentry/UMEntryOperation.cpp
  src/entrys/umentry/UMEntry.cpp
  src/entrys/umentry/UMEntryCreationOp.cpp
  src/entrys/umentry/UMEntryRenewalOp.cpp
  src/entrys/umentry/UMEntryDeletionOp.cpp
  src/entrys/umentry/UMEntryUpdateOp.cpp
  src/entrys/umentry/UMEntryOwnershipTransferOp.cpp
  src/entrys/uentry/UniqueEntry.cpp
  src/entrys/uentry/UniqueEntryOperation.cpp
  src/entrys/uentry/UniqueEntryCreationOp.cpp
  src/entrys/uentry/UniqueEntryDeletionOp.cpp
  src/entrys/uentry/UniqueEntryRenewalOp.cpp
  src/entrys/uentry/UniqueEntryOwnershipTransferOp.cpp
  src/entrys/token/UtilityToken.cpp
  src/entrys/token/UtilityTokenOperation.cpp
  src/entrys/token/UtilityTokenCreationOp.cpp
  src/entrys/token/UtilityTokenDeletionOp.cpp
  src/entrys/token/UtilityTokenOwnershipTransferOp.cpp
  src/core/Block.cpp
  src/core/Coin.cpp
  src/daemon/ReadOnlyDaemonBase.cpp
  src/daemon/WriteOnlyDaemonBase.cpp
  src/daemon/odin/ReadOnlyOdinDaemon.cpp
  src/daemon/odin/ReadWriteOdinDaemon.cpp
  src/core/Transaction.cpp
  src/lookup/UMEntryLookup.cpp
  src/lookup/UtilityTokenLookup.cpp
  src/lookup/UniqueEntryLookup.cpp
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
target_include_directories(forge-src PUBLIC
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
target_link_libraries(forge-src LINK_PUBLIC
  fmt
  jsonrpc::client
  jsonrpc::server
  jsoncpp
  g3logger
  stdc++fs #TODO: this is horrible to do imo
  ${CMAKE_THREAD_LIBS_INIT})

#executable forged
add_executable(forged
  src/forged.cpp)

#executable forge-cli
add_executable(forge-cli
  src/forge-cli.cpp)


#make sure everything is build in the right order
add_dependencies(forge-src fmt-project)
add_dependencies(forge-src jsonrpc-project)
add_dependencies(forge-src g3logger-project)
add_dependencies(forge-src CLI11-project)
add_dependencies(forge-src utilxx-project)
add_dependencies(forge-src cpptoml-project)
add_dependencies(forged forge-src)
add_dependencies(forge-cli forge-src)


# make headers available in forged.cpp
target_include_directories(forged PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  fmt
  ${CXXOPTS_INCLUDE_DIR})

#link forged.cpp against fmt and thread lib
target_link_libraries(forged LINK_PUBLIC
  forge-src
  fmt
  jsonrpc::server
  ${CMAKE_THREAD_LIBS_INIT})

# make headers available in forge-cli.cpp
target_include_directories(forge-cli PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CLI11_INCLUDE_DIR}
  fmt)

#link forge-cli.cpp against fmt and thread lib
target_link_libraries(forge-cli LINK_PUBLIC
  forge-src
  fmt
  ${CMAKE_THREAD_LIBS_INIT})



if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
