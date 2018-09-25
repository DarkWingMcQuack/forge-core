cmake_minimum_required(VERSION 3.9)

project(cppBUDDY)

#debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g3 -ggdb")

#release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -march=native -fstack-protector-strong")

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
  main.cpp)

#use c++17 as standard
set_target_properties(cppBUDDY PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF)

# make headers available
target_include_directories(cppBUDDY PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIR})

#link against boost and thread libs
target_link_libraries(cppBUDDY LINK_PUBLIC
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})
