set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(unit_tests
  main.cpp
  transaction_tests.cpp
  block_tests.cpp)

target_link_libraries(unit_tests LINK_PUBLIC
  gtest
  buddy-src
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
