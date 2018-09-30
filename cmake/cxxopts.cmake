include(ExternalProject)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_STATIC_LIBS=ON
  -DCXXOPTS_BUILD_EXAMPLES=OFF
  -DBUILD_SHARED_LIBS=OFF)

ExternalProject_Add(cxxopts-project
  PREFIX deps/cxxopts
  DOWNLOAD_NAME cxxopts-2.1.1.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/jarro2783/cxxopts/archive/v2.1.1.tar.gz
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(cxxopts-project INSTALL_DIR)
add_library(cxxopts STATIC IMPORTED)
set(CXXOPTS_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cxxopts${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CXXOPTS_INCLUDE_DIR ${INSTALL_DIR}/include)
set_property(TARGET cxxopts PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CXXOPTS_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
