set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(unit_tests
  main.cpp
  transaction_tests.cpp
  block_tests.cpp
  entry_tests.cpp
  utility_token_tests.cpp
  entry_lookup_tests.cpp
  umentry_operation_tests.cpp
  utility_token_operation_tests.cpp
  utility_token_lookup_tests.cpp
  read_only_odin_tests.cpp
  read_write_odin_tests.cpp)

target_link_libraries(unit_tests LINK_PUBLIC
  gtest
  forge-src
  )

target_include_directories(
  unit_tests PUBLIC
  gtest
  jsoncpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )

add_test(
  NAME unit_tests
  COMMAND
  ${CMAKE_BINARY_DIR}/test/unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/
  )
