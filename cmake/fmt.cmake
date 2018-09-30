include(ExternalProject)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_STATIC_LIBS=ON
  -DBUILD_SHARED_LIBS=OFF
  -DFMT_TEST:BOOL=OFF
  -DFMT_INSTALL:BOOL=ON
  -DFMT_DOC:BOOL=OFF)

ExternalProject_Add(fmt-project
  PREFIX deps/fmt
  DOWNLOAD_NAME fmt-5.2.1.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/fmtlib/fmt/archive/5.2.1.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(fmt-project INSTALL_DIR)
add_library(fmt STATIC IMPORTED)
set(FMT_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}fmt${CMAKE_STATIC_LIBRARY_SUFFIX})
set(FMT_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${FMT_INCLUDE_DIR})  # Must exist.
set_property(TARGET fmt PROPERTY IMPORTED_LOCATION ${FMT_LIBRARY})
set_property(TARGET fmt PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${FMT_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
