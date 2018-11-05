include(ExternalProject)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DCPPTOML_BUILD_EXAMPLES=OFF
  -DENABLE_LIBCXX=OFF)


ExternalProject_Add(cpptoml-project
  PREFIX deps/cpptoml
  DOWNLOAD_NAME cpptoml-0.1.1.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/skystrife/cpptoml/archive/v0.1.1.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(cpptoml-project INSTALL_DIR)
set(CPPTOML_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
