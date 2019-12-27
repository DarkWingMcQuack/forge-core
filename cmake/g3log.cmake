include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_STATIC_LIBS=ON
  -DBUILD_SHARED_LIBS=OFF
  -DG3_SHARED_LIB=OFF
  -DUSE_DYNAMIC_LOGGING_LEVELS=ON
  -DADD_FATAL_EXAMPLE=OFF)

ExternalProject_Add(g3logger-project
  PREFIX deps/g3logger
  DOWNLOAD_NAME g3logger-1.3.2.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/KjellKod/g3log/archive/1.3.2.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(g3logger-project INSTALL_DIR)
add_library(g3logger STATIC IMPORTED)
set(G3LOGGER_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}g3logger${CMAKE_STATIC_LIBRARY_SUFFIX})
set(G3LOGGER_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${G3LOGGER_INCLUDE_DIR})  # Must exist.
set_property(TARGET g3logger PROPERTY IMPORTED_LOCATION ${G3LOGGER_LIBRARY})
set_property(TARGET g3logger PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${G3LOGGER_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
